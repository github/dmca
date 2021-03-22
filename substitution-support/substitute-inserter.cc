#include <Scope.hpp>
#include <bootstrap.h>
#include <codesign.h>
#include <common.h>
#include <cs_blobs.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <libproc.h>
#include <mach-o/dyld.h>
#include <mach-o/dyld_images.h>
#include <mach-o/getsect.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <os/log.h>
#include <pthread.h>
#include <spawn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <syslog.h>
#include <ubc_headers.h>
#include <unistd.h>
#include <xpc/xpc.h>
extern "C" {
#include "substitute.h"
#include "substitute_daemonUser.h"
#include <sandbox.h>
#include <sandbox_private.h>
char ***_NSGetArgv(void);
char ***_NSGetEnviron(void);
#define environ (*_NSGetEnviron())
int reboot(int mode);
int reboot3(int mode);
#define RB_NOSYNC 0x04
void *xpc_create_from_plist(void *data, size_t size);
int sandbox_check_by_audit_token(audit_token_t *token, const char *operation, int type, ...);
const struct mach_header_64 *_NSGetMachExecuteHeader(void);
}

#define SubHook(function, replacement, old_ptr) ((struct substitute_function_hook){(void *)function, (void *)replacement, (void *)old_ptr})

#define INSERTER "/usr/lib/substitute-inserter.dylib"
#define K_DYLD_INSERT_LIBRARIES "DYLD_INSERT_LIBRARIES="

static mach_port_t get_server_port(void) {
	mach_port_t port = MACH_PORT_NULL;
	kern_return_t kr = task_get_special_port(mach_task_self(), TASK_SEATBELT_PORT, &port);
	if (kr != KERN_SUCCESS || !MACH_PORT_VALID(port)) {
		error_log("Unable to get server port (task_get_special_port: %s, port: %x)", mach_error_string(kr), port);
		return MACH_PORT_NULL;
	}
	return port;
}

static int $posix_spawn_generic(pid_t *pid, const char *path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *attrp, char *const argv[], char *const envp[], __typeof__(posix_spawn) *old) {
	if (path == NULL || argv == NULL) {
		return old(pid, path, file_actions, attrp, argv, envp);
	}
	if (envp == NULL)
		envp = environ;
	char resolved[MAXPATHLEN];
	char *real = realpath(path, resolved);
	if (real == NULL) {
		return old(pid, path, file_actions, attrp, argv, envp);
	}
	mach_port_t port = get_server_port();
	if (port == MACH_PORT_NULL) {
		return old(pid, path, file_actions, attrp, argv, envp);
	}
	_scope({ mach_port_deallocate(mach_task_self(), port); });
	kern_return_t kr = substitute_preflight(port, mach_task_self(), (mach_vm_address_t)real, (mach_vm_size_t)(strlen(real) + 1));
	if (kr != KERN_SUCCESS) {
		error_log("Unable to preflight %s: %s", real, mach_error_string(kr));
		return old(pid, path, file_actions, attrp, argv, envp);
	}
	struct stat st;
	if (lstat(real, &st) == -1) {
		return old(pid, path, file_actions, attrp, argv, envp);
	}
	pid_t child = 0;
	if (pid == NULL) {
		pid = &child;
	}
	posix_spawnattr_t attr = NULL;
	short flags = 0;
	if (attrp == NULL) {
		if (posix_spawnattr_init(&attr) != 0)
			return old(pid, path, file_actions, attrp, argv, envp);
		attrp = &attr;
	} else {
		if (posix_spawnattr_getflags(attrp, &flags) != 0)
			return old(pid, path, file_actions, attrp, argv, envp);
	}
	bool is_setugid = ((st.st_mode & S_ISUID) || (st.st_mode & S_ISGID));
	if (is_setugid) {
		if (posix_spawnattr_setflags((posix_spawnattr_t *)attrp, flags | POSIX_SPAWN_START_SUSPENDED) != 0) {
			if (attrp == &attr)
				posix_spawnattr_destroy(&attr);
			return old(pid, path, file_actions, attrp, argv, envp);
		}
	}
	bool set_exec = (flags & POSIX_SPAWN_SETEXEC);
	bool start_suspended = (flags & POSIX_SPAWN_START_SUSPENDED);
	size_t nenv = 0;
	size_t ninsert = 0;
	const char *insert = NULL;
	for (char *const *ptr = envp; *ptr != NULL; ++ptr, ++nenv) {
		if (insert == NULL && strncmp(*ptr, K_DYLD_INSERT_LIBRARIES, strlen(K_DYLD_INSERT_LIBRARIES)) == 0) {
			insert = *ptr;
			ninsert = nenv;
		}
	}
	size_t newnenv = nenv;
	if (insert == NULL) {
		ninsert = newnenv++;
	}
	const char *newenvp[(newnenv + 1) * sizeof(char *)];
	memcpy(newenvp, envp, nenv * sizeof(char *));
	char *newInsert;
	if (insert == NULL) {
		newInsert = strdup(K_DYLD_INSERT_LIBRARIES INSERTER);
	} else {
		newInsert = (char *)malloc(strlen(insert) + strlen(INSERTER) + 2);
		char *writePtr = newInsert;
		memcpy(writePtr, K_DYLD_INSERT_LIBRARIES, strlen(K_DYLD_INSERT_LIBRARIES) + 1);
		writePtr += strlen(K_DYLD_INSERT_LIBRARIES);
		memcpy(writePtr, INSERTER, strlen(INSERTER) + 1);
		writePtr += strlen(INSERTER);

		// Clean out any extra existing references
		char *next;
		for (char *existing = (char *)strchr(insert, '='); existing++; existing = next) {
			next = strchr(existing, ':');
			if (next)
				*next = '\0';

			if (strcmp(existing, INSERTER) == 0)
				continue;

			*writePtr++ = ':';
			strcpy(writePtr, existing);
			writePtr += strlen(existing);
		}
	}
	newenvp[ninsert] = newInsert;
	newenvp[newnenv] = NULL;
	if ((is_setugid && set_exec) && (kr = substitute_setup_process(port, getpid(), set_exec, !start_suspended)) != KERN_SUCCESS) {
		error_log("substitute_unrestrict: %s", mach_error_string(kr));
		if (attrp == &attr)
			posix_spawnattr_destroy(&attr);
		free(newInsert);
		return old(pid, path, file_actions, attrp, argv, envp);
	}
	int value = old(pid, path, file_actions, attrp, argv, (char **)newenvp);
	if (attrp == &attr)
		posix_spawnattr_destroy(&attr);
	free(newInsert);
	if (value != 0 || set_exec) {
		return value;
	}
	if (is_setugid) {
		kr = substitute_setup_process(port, *pid, set_exec, !start_suspended);
		if (kr != KERN_SUCCESS) {
			error_log("substitute_unrestrict: %s", mach_error_string(kr));
		}
	}
	return 0;
}

static int (*old_posix_spawn)(pid_t *__restrict pid, const char *__restrict path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *__restrict attrp, char *const __argv[__restrict], char *const __envp[__restrict]) = NULL;
static int hook_posix_spawn(pid_t *__restrict pid, const char *__restrict path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *__restrict attrp, char *const __argv[__restrict], char *const __envp[__restrict]) {
	return $posix_spawn_generic(pid, path, file_actions, attrp, __argv, __envp, old_posix_spawn);
}

static int (*old_posix_spawnp)(pid_t *__restrict pid, const char *__restrict path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *__restrict attrp, char *const __argv[__restrict], char *const __envp[__restrict]) = NULL;
static int hook_posix_spawnp(pid_t *__restrict pid, const char *__restrict path, const posix_spawn_file_actions_t *file_actions, const posix_spawnattr_t *__restrict attrp, char *const __argv[__restrict], char *const __envp[__restrict]) {
	return $posix_spawn_generic(pid, path, file_actions, attrp, __argv, __envp, old_posix_spawnp);
}

static int hook_execve(const char *__file, char *const *__argv, char *const *__envp) {
	posix_spawnattr_t attr = NULL;
	posix_spawnattr_init(&attr);
	short flags = 0;
	posix_spawnattr_getflags(&attr, &flags);
	posix_spawnattr_setflags(&attr, flags | POSIX_SPAWN_SETEXEC);
	int value = posix_spawn(NULL, __file, NULL, &attr, __argv, __envp);
	if (value != 0) {
		errno = value;
	}
	posix_spawnattr_destroy(&attr);
	return -1;
}

static __typeof__(csops) *old_csops = NULL;
static int hook_csops(pid_t pid, unsigned int ops, void *useraddr, size_t usersize) {
	int ret = old_csops(pid, ops, useraddr, usersize);
	if (ret == 0 && ops == CS_OPS_STATUS && useraddr != NULL) {
		*(uint32_t *)useraddr |= (CS_VALID | CS_PLATFORM_BINARY);
	}
	return ret;
}

static __typeof__(csops_audittoken) *old_csops_audittoken = NULL;
static int hook_csops_audittoken(pid_t pid, unsigned int ops, void *useraddr, size_t usersize, audit_token_t *token) {
	int ret = old_csops_audittoken(pid, ops, useraddr, usersize, token);
	if (ret == 0 && ops == CS_OPS_STATUS && useraddr != NULL) {
		*(uint32_t *)useraddr |= (CS_VALID | CS_PLATFORM_BINARY);
	}
	return ret;
}

static __typeof__(fcntl) *old__fcntl = NULL;
static int hook__fcntl(int fildes, int cmd, ...) {
	if (cmd == F_ADDFILESIGS_RETURN) {
		return -1;
	}
	if (cmd == F_CHECK_LV) {
		return 0;
	}
	long arg;
	va_list args;
	va_start(args, cmd);
	arg = (long)va_arg(args, long);
	va_end(args);
	return old__fcntl(fildes, cmd, arg);
}

static __typeof__(fcntl) *old_fcntl = NULL;
static int hook_fcntl(int fildes, int cmd, ...) {
	if (cmd == F_CHECK_LV) {
		errno = 0;
		return 0;
	}
	long arg;
	va_list args;
	va_start(args, cmd);
	arg = (long)va_arg(args, long);
	va_end(args);
	return old_fcntl(fildes, cmd, arg);
}

static __typeof__(host_set_special_port) *old_host_set_special_port = NULL;
static kern_return_t hook_host_set_special_port(host_priv_t host_priv, int which, mach_port_t port) {
	if (which == HOST_AMFID_PORT || which == HOST_KEXTD_PORT) {
		return KERN_FAILURE;
	}
	return old_host_set_special_port(host_priv, which, port);
}

static __typeof__(xpc_dictionary_get_value) *old_xpc_dictionary_get_value = NULL;
static xpc_object_t hook_xpc_dictionary_get_value(xpc_object_t xdict, const char *key) {
	static bool continuedBooting = false;
	static xpc_object_t realBootstrapPaths = NULL;
	static xpc_object_t fakeBootstrapPaths = NULL;
	xpc_object_t retval = old_xpc_dictionary_get_value(xdict, key);
	if (!continuedBooting && retval != NULL) {
		if (strcmp(key, "LaunchDaemons") == 0) {
			CFTypeRef keys[] = {
				CFSTR("Label"),
				CFSTR("Program"),
				CFSTR("UserName"),
				CFSTR("RunAtLoad"),
				CFSTR("KeepAlive"),
			};
			CFTypeRef values[] = {
				CFSTR("science.xnu.substitute.reload"),
				CFSTR("/usr/libexec/substitute-reload"),
				CFSTR("root"),
				kCFBooleanTrue,
				kCFBooleanFalse,
			};
			const char *plistPath = "/Library/Caches/science.xnu.substitute.reload.plist";
			FILE *plistFile = fopen(plistPath, "w");
			if (plistFile == NULL) {
				panic_log("Unable to open %s for writing: %s", plistPath, strerror(errno));
			}
			_scope({ fclose(plistFile); });
			CFDictionaryRef plistDict = CFDictionaryCreate(kCFAllocatorDefault, keys, values, 5, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
			_scope({ CFRelease(plistDict); });
			CFDataRef plistData = CFPropertyListCreateData(kCFAllocatorDefault, (CFPropertyListRef)plistDict, kCFPropertyListXMLFormat_v1_0, 0, NULL);
			_scope({ CFRelease(plistData); });
			void *plistDataPtr = (void *)CFDataGetBytePtr(plistData);
			size_t plistDataSize = (size_t)CFDataGetLength(plistData);
			fwrite(plistDataPtr, plistDataSize, 1, plistFile);
			fflush(plistFile);
			xpc_object_t xpcDict = xpc_create_from_plist(plistDataPtr, plistDataSize);
			_scope({ xpc_release(xpcDict); });
			xpc_dictionary_set_value(retval, plistPath, xpcDict);
		} else if (strcmp(key, "Paths") == 0) {
			realBootstrapPaths = xpc_retain(retval);
			fakeBootstrapPaths = xpc_array_create(NULL, 0);
			xpc_array_set_string(fakeBootstrapPaths, XPC_ARRAY_APPEND, "/Library/Caches");
			return fakeBootstrapPaths;
		} else if (strcmp(key, "paths") == 0) {
			if (xpc_array_get_count(retval) > 0) {
				const char *firstString = xpc_array_get_string(retval, 0);
				if (firstString != NULL && strcmp(firstString, "continue-booting") == 0) {
					if (realBootstrapPaths != NULL) {
						xpc_array_apply(realBootstrapPaths, ^bool(size_t index, xpc_object_t value) {
							xpc_array_append_value(fakeBootstrapPaths, value);
							xpc_array_append_value(retval, value);
							return true;
						});
						xpc_release(realBootstrapPaths);
						realBootstrapPaths = NULL;
					}
					continuedBooting = true;
				}
			}
		}
	}
	return retval;
}

static __typeof__(remove) *old_remove = NULL;

static int hook_remove(const char *path) {
	if (path == NULL) {
		return old_remove(path);
	}
	char resolved[PATH_MAX];
	char *real = realpath(path, resolved);
	if (real == NULL) {
		return old_remove(path);
	}
	if (strcmp(real, "/private/var/tmp/slide.txt") == 0 || strncmp(real, "/private/var/tmp/.substitute_", strlen("/private/var/tmp/.substitute_")) == 0 || strncmp(real, "/private/var/tmp/.loader_", strlen("/private/var/tmp/.loader_")) == 0) {
		errno = EPERM;
		return -1;
	}
	return old_remove(path);
}

static __typeof__(sandbox_check_by_audit_token) *old_sandbox_check_by_audit_token = NULL;
static int hook_sandbox_check_by_audit_token(audit_token_t *token, const char *operation, int type, ...) {
	va_list ap;
	va_start(ap, type);
	long args[5];
	for (int i = 0; i < 5; i++)
		args[i] = va_arg(ap, long);
	va_end(ap);
	if (strcmp(operation, "mach-lookup") == 0 || strcmp(operation, "mach-register") == 0) {
		const char *name = (char *)args[0];
		if (strncmp(name, "cy:", strlen("cy:")) == 0) {
			/* always allow */
			return 0;
		}
	}
	return old_sandbox_check_by_audit_token(token, operation, type, args[0], args[1], args[2], args[3], args[4]);
}

__attribute__((constructor)) void init(void) {
	mach_port_t port = get_server_port();
	if (port == MACH_PORT_NULL) {
		panic_log("Unable to get server port");
	}
	_scope({ mach_port_deallocate(mach_task_self(), port); });
	int rv = 0;
	uint32_t csflags = 0;
	csops(getpid(), CS_OPS_STATUS, &csflags, sizeof(csflags));
	if ((csflags & (CS_DEBUGGED | CS_HARD | CS_KILL)) != CS_DEBUGGED) {
		rv = substitute_setup_self(port);
		if (rv != KERN_SUCCESS) {
			panic_log("Unable to setup myself: %s", mach_error_string(rv));
		}
	}
	if (sandbox_check(getpid(), NULL, (enum sandbox_filter_type)(SANDBOX_FILTER_NONE | SANDBOX_CHECK_NO_REPORT)) > 0) {
		mach_vm_address_t stringAddress = 0;
		mach_vm_size_t stringSize = 0;
		rv = substitute_copy_extensions(port, mach_task_self(), &stringAddress, &stringSize);
		if (rv != KERN_SUCCESS) {
			panic_log("Unable to copy sandbox extensions: %s", mach_error_string(rv));
		}
		_scope({ mach_vm_deallocate(mach_task_self(), stringAddress, stringSize); });
		char *string = (char *)stringAddress;
		char *token;
		while ((token = strsep(&string, ":"))) {
			if (sandbox_extension_consume(token) == -1) {
				panic_log("Unable to consume extension: %s", token);
			}
		}
	}
	char path[MAXPATHLEN * 4];
	memset(path, '\0', sizeof(path));
	proc_pidpath(getpid(), path, sizeof(path));
	size_t n_hooks = 0;
	struct substitute_function_hook hooks[13];
	struct task_dyld_info dyld_info;
	mach_msg_type_number_t cnt = TASK_DYLD_INFO_COUNT;
	rv = task_info(mach_task_self(), TASK_DYLD_INFO, (task_info_t)&dyld_info, &cnt);
	if (rv == KERN_SUCCESS) {
		struct dyld_all_image_infos *all_image_info = (struct dyld_all_image_infos *)dyld_info.all_image_info_addr;
		const struct mach_header *dyld_load_addr = all_image_info->dyldImageLoadAddress;
		struct substitute_image dyld = {-1, NULL, dyld_load_addr};
		void *sym = NULL;
		const char *name = "_fcntl";
		substitute_find_private_syms(&dyld, &name, &sym, 1);
		if (sym != NULL) {
			hooks[n_hooks++] = SubHook(sym, &hook__fcntl, &old__fcntl);
		}
	}
	hooks[n_hooks++] = SubHook(&fcntl, &hook_fcntl, &old_fcntl);
	hooks[n_hooks++] = SubHook(&posix_spawn, &hook_posix_spawn, &old_posix_spawn);
	hooks[n_hooks++] = SubHook(&posix_spawnp, &hook_posix_spawnp, &old_posix_spawnp);
	hooks[n_hooks++] = SubHook(&vfork, &fork, NULL);
	hooks[n_hooks++] = SubHook(&execve, &hook_execve, NULL);
	hooks[n_hooks++] = SubHook(&csops, &hook_csops, &old_csops);
	hooks[n_hooks++] = SubHook(&csops_audittoken, &hook_csops_audittoken, &old_csops_audittoken);
	if (getpid() == 1) {
		hooks[n_hooks++] = SubHook(&host_set_special_port, &hook_host_set_special_port, &old_host_set_special_port);
		hooks[n_hooks++] = SubHook(&xpc_dictionary_get_value, &hook_xpc_dictionary_get_value, &old_xpc_dictionary_get_value);
		hooks[n_hooks++] = SubHook(&remove, &hook_remove, &old_remove);
		hooks[n_hooks++] = SubHook(&sandbox_check_by_audit_token, &hook_sandbox_check_by_audit_token, &old_sandbox_check_by_audit_token);
	}
	rv = substitute_hook_functions(hooks, n_hooks, NULL, SUBSTITUTE_NO_THREAD_SAFETY);
	if (rv != SUBSTITUTE_OK) {
		panic_log("Unable to install mandatory system hooks: %s", substitute_strerror(rv));
	}
	if (getpid() == 1) {
		dlopen("/System/Library/Caches/com.apple.xpcd/xpcd_cache.dylib", RTLD_NOW);
	} else if (strcmp(path, "/usr/bin/powerlogHelperd") == 0) {
		return;
	}
	uint32_t count = _dyld_image_count();
	for (uint32_t i = 0; i < count; ++i) {
		const char *name = _dyld_get_image_name(i);
		if (strcmp(name, "/System/Library/Frameworks/Foundation.framework/Foundation") == 0) {
			dlopen(name, RTLD_LAZY | RTLD_GLOBAL);
			break;
		}
	}
	if (access("/var/tmp/.substitute_disable_loader", F_OK) != 0) {
		dlopen("/usr/lib/libsubstitute.dylib", RTLD_LAZY | RTLD_LOCAL);
		dlopen("/usr/lib/substitute-loader.dylib", RTLD_LAZY | RTLD_GLOBAL);
	}
}
