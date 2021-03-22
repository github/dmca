#include "CSCommon.h"
#include "common.h"
#include "cs_bypass.h"
#include <Scope.hpp>
#include <bootstrap.h>
#include <copyfile.h>
#include <cs_blobs.h>
#include <dlfcn.h>
#include <libproc.h>
#include <mach-o/dyld.h>
#include <mach-o/dyld_images.h>
#include <mach-o/fat.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <pthread.h>
#include <spawn.h>
#include <string.h>
#include <sys/semaphore.h>
#include <sys/sysctl.h>
#include <substitute.h>
#include <ubc_headers.h>
extern "C" {
#include "amfidServer.h"
#include "substitute-injector.h"
#include "substitute_daemonServer.h"
#include "substituted-types.h"
#include <offsetcache.h>
#include <sandbox_private.h>
typedef boolean_t (*dispatch_mig_callback_t)(mach_msg_header_t *message, mach_msg_header_t *reply);
mach_msg_return_t dispatch_mig_server(dispatch_source_t ds, size_t maxmsgsz, dispatch_mig_callback_t callback);
}

#define MAX_CHUNK_SIZE 0xfff // MIG limitation
#define	P_SUGID 0x00000100 /* Has set privileges since last exec */
#define P_REBOOT 0x00200000	   /* Process called reboot() */
#define TF_PLATFORM 0x00000400 /* task is a platform binary */
#define TF_CORPSE 0x00000020 /* task is a corpse */
#define CS_MAX_BLOB_SIZE (40ULL * 1024ULL * 1024ULL)
#define INSERTER_PATH "/usr/lib/substitute-inserter.dylib"
#define SEMAPHORE_NAME "/science.xnu.substituted"

typedef struct {
	mach_vm_address_t allproc;
	mach_vm_address_t cs_blob_generation_count;
	mach_vm_address_t container_profile;
	mach_vm_size_t proc_p_pid;
	mach_vm_size_t proc_p_ucred;
	mach_vm_size_t proc_p_svuid;
	mach_vm_size_t proc_p_svgid;
	mach_vm_size_t proc_p_csflags;
	mach_vm_size_t proc_p_textvp;
	mach_vm_size_t proc_p_flag;
	mach_vm_size_t proc_p_fd;
	mach_vm_size_t filedesc_fd_ofiles;
	mach_vm_size_t fileproc_f_fglob;
	mach_vm_size_t fileglob_fg_data;
	mach_vm_size_t proc_task;
	mach_vm_size_t ucred_cr_svuid;
	mach_vm_size_t ucred_cr_uid;
	mach_vm_size_t ucred_cr_svgid;
	mach_vm_size_t ucred_cr_groups;
	mach_vm_size_t ucred_cr_label;
	mach_vm_size_t label_sandbox;
	mach_vm_size_t vnode_v_ubcinfo;
	mach_vm_size_t ubc_info_csblobs;
	mach_vm_size_t task_itk_space;
	mach_vm_size_t ipc_space_is_table;
	mach_vm_size_t size_ipc_entry;
	mach_vm_size_t ipc_port_ip_kobject;
	mach_vm_size_t host_special;
	mach_vm_size_t task_t_flags;
	mach_vm_size_t ipc_space_is_task;
	mach_vm_size_t task_bsd_info;
} kernel_offsets_t;
typedef struct {
	mach_vm_address_t xpcproxy;
	mach_vm_address_t proxyless;
} launchd_offsets_t;
typedef struct {
	uint8_t hash[CS_CDHASH_LEN];
	uint8_t hook[CS_CDHASH_LEN];
} cdhash_hook_t;
cdhash_hook_t *cdhash_hooks = NULL;
size_t n_cdhash_hooks = 0;
pthread_mutex_t cdhash_hooks_lock = PTHREAD_MUTEX_INITIALIZER;
cpu_type_t cpu_type = -1;
mach_port_t our_port = MACH_PORT_NULL;
mach_port_t amfid_port = MACH_PORT_NULL;
mach_port_t fake_amfid_port = MACH_PORT_NULL;
task_t tfp0 = TASK_NULL;
uint32_t kernel_slide = 0;
mach_vm_address_t offset_cache = 0;
uint8_t fake_cdhash[CS_CDHASH_LEN];
uint8_t real_cdhash[CS_CDHASH_LEN];
bool forged_cdhash = false;
kernel_offsets_t koffsets = {0};
launchd_offsets_t loffsets = {0};
dispatch_queue_t amfid_queue = NULL;
dispatch_source_t amfid_source = NULL;
dispatch_queue_t our_queue = NULL;
dispatch_source_t our_source = NULL;
mach_vm_address_t current_proc;
char *sandbox_extensions = NULL;
char ourPath[PROC_PIDPATHINFO_MAXSIZE] = {0};

bool cdhash_hooks_add(uint8_t hash[CS_CDHASH_LEN], uint8_t hook[CS_CDHASH_LEN]) {
	pthread_mutex_lock(&cdhash_hooks_lock);
	_scope({ pthread_mutex_unlock(&cdhash_hooks_lock); });
	if (cdhash_hooks == NULL) {
		cdhash_hooks = (__typeof__(cdhash_hooks))malloc(sizeof(*cdhash_hooks));
		if (cdhash_hooks == NULL) {
			error_log("Unable to allocate memory for cdhash hooks: %s", strerror(errno));
			return false;
		}
	} else {
		auto new_cdhash_hooks = (__typeof__(cdhash_hooks))realloc(cdhash_hooks, sizeof(*cdhash_hooks) * (n_cdhash_hooks + 1));
		if (new_cdhash_hooks == NULL) {
			error_log("Unable to reallocate memory for cdhash hooks: %s", strerror(errno));
			return false;
		}
		cdhash_hooks = new_cdhash_hooks;
	}
	cdhash_hook_t *cdhash_hook = &cdhash_hooks[n_cdhash_hooks++];
	memcpy(cdhash_hook->hash, hash, CS_CDHASH_LEN);
	memcpy(cdhash_hook->hook, hook, CS_CDHASH_LEN);
	return true;
}

bool cdhash_hook_get(uint8_t hash[CS_CDHASH_LEN], uint8_t hook[CS_CDHASH_LEN]) {
	pthread_mutex_lock(&cdhash_hooks_lock);
	_scope({ pthread_mutex_unlock(&cdhash_hooks_lock); });
	for (size_t i = 0; i < n_cdhash_hooks; i++) {
		cdhash_hook_t *cdhash_hook = &cdhash_hooks[i];
		if (memcmp(cdhash_hook->hash, hash, CS_CDHASH_LEN) == 0) {
			memcpy(hook, cdhash_hook->hook, CS_CDHASH_LEN);
			return true;
		}
	}
	return false;
}

bool cdhash_hook_remove(uint8_t hash[CS_CDHASH_LEN]) {
	pthread_mutex_lock(&cdhash_hooks_lock);
	_scope({ pthread_mutex_unlock(&cdhash_hooks_lock); });
	int entry_index = -1;
	for (size_t i = 0; i < n_cdhash_hooks; i++) {
		cdhash_hook_t *hook = &cdhash_hooks[i];
		if (memcmp(hook->hash, hash, CS_CDHASH_LEN) == 0) {
			entry_index = i;
			break;
		}
	}
	if (entry_index == -1) {
		return true;
	}
	cdhash_hook_t *new_cdhash_hooks = NULL;
	size_t new_n_cdhash_hooks = 0;
	if (n_cdhash_hooks > 1) {
		new_n_cdhash_hooks = n_cdhash_hooks - 1;
		new_cdhash_hooks = (__typeof__(new_cdhash_hooks))malloc(sizeof(*new_cdhash_hooks) * new_n_cdhash_hooks);
		if (new_cdhash_hooks == NULL) {
			error_log("Unable to allocate memory foe new cdhash hooks: %s", strerror(errno));
			return false;
		}
		size_t index = 0;
		for (size_t i = 0; i < n_cdhash_hooks; i++) {
			if (i == entry_index) {
				continue;
			}
			memcpy(&new_cdhash_hooks[index++], &cdhash_hooks[i], sizeof(*cdhash_hooks));
		}
		free(cdhash_hooks);
	} else {
		new_n_cdhash_hooks = 0;
		free(cdhash_hooks);
		new_cdhash_hooks = NULL;
	}
	cdhash_hooks = new_cdhash_hooks;
	n_cdhash_hooks = new_n_cdhash_hooks;
	return true;
}

mach_vm_address_t memory_allocate(task_t task, mach_vm_size_t size) {
	mach_vm_address_t address = 0;
	kern_return_t kr = mach_vm_allocate(task, &address, size, VM_FLAGS_ANYWHERE);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to allocate memory: %s", mach_error_string(kr));
		address = 0;
	}
	return address;
}

bool memory_free(task_t task, mach_vm_address_t address, mach_vm_size_t size) {
	kern_return_t kr = mach_vm_deallocate(task, address, size);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to free memory: %s", mach_error_string(kr));
		return false;
	}
	return true;
}

void *memory_remap(task_t task, mach_vm_address_t address, mach_vm_size_t size) {
	mach_vm_address_t target_address = 0;
	vm_prot_t cur_prot, max_prot;
	kern_return_t kr = mach_vm_remap(mach_task_self(), &target_address, size, 0, VM_FLAGS_ANYWHERE | VM_FLAGS_RETURN_DATA_ADDR, task, address, FALSE, &cur_prot, &max_prot, VM_INHERIT_NONE);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to remap memory: %s", mach_error_string(kr));
		return NULL;
	}
	return (void *)target_address;
}

bool memory_read(task_t task, mach_vm_address_t address, void *buffer, mach_vm_size_t size) {
	kern_return_t kr;
	mach_vm_size_t offset = 0;
	while (offset < size) {
		mach_vm_size_t sz = 0;
		mach_vm_size_t chunk = MAX_CHUNK_SIZE;
		if (chunk > size - offset)
			chunk = size - offset;
		kr = mach_vm_read_overwrite(task, address + offset, chunk, (mach_vm_address_t)buffer + offset, &sz);
		if (kr != KERN_SUCCESS) {
			error_log("Unable to read memory: %s", mach_error_string(kr));
			return false;
		}
		offset += sz;
	}
	if (offset != size) {
		error_log("Partial read: %llu of %llu bytes", offset, size);
		bzero(buffer, size);
		return false;
	}
	return true;
}

void *memory_copy_data(task_t task, mach_vm_address_t address, mach_vm_size_t size) {
	void *buffer = malloc(size);
	if (buffer != NULL) {
		if (memory_read(task, address, buffer, size)) {
			return buffer;
		}
		free(buffer);
	}
	return NULL;
}

mach_vm_address_t memory_read_ptr(mach_port_t task, mach_vm_address_t address) {
	mach_vm_address_t val = 0;
	memory_read(task, address, (void *)&val, sizeof(val));
	return val;
}

uint32_t memory_read_32(mach_port_t task, mach_vm_address_t address) {
	uint32_t val = 0;
	memory_read(task, address, (void *)&val, sizeof(val));
	return val;
}

uint16_t memory_read_16(mach_port_t task, mach_vm_address_t address) {
	uint16_t val = 0;
	memory_read(task, address, (void *)&val, sizeof(val));
	return val;
}

uint8_t memory_read_8(mach_port_t task, mach_vm_address_t address) {
	uint8_t val = 0;
	memory_read(task, address, (void *)&val, sizeof(val));
	return val;
}

bool memory_write(mach_port_t task, mach_vm_address_t address, void *buffer, mach_vm_size_t size) {
	kern_return_t kr = KERN_FAILURE;
	mach_vm_size_t offset = 0;
	while (offset < size) {
		mach_vm_size_t chunk = MAX_CHUNK_SIZE;
		if (chunk > size - offset)
			chunk = size - offset;
		kr = mach_vm_write(task, address + offset, (mach_vm_address_t)buffer + offset, (mach_msg_type_number_t)chunk);
		if (kr != KERN_SUCCESS) {
			error_log("Unable to write memory: %s", mach_error_string(kr));
			return false;
		}
		offset += chunk;
	}
	if (offset != size) {
		error_log("Partial write: %llu of %llu bytes", offset, size);
		return false;
	}
	return true;
}

bool memory_copy(mach_port_t task, mach_vm_address_t from, mach_vm_address_t to, mach_vm_size_t size) {
	kern_return_t kr = mach_vm_copy(task, from, size, to);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to copy memory: %s", mach_error_string(kr));
		return false;
	}
	return true;
}

bool memory_protect(mach_port_t task, mach_vm_address_t address, mach_vm_size_t size, vm_prot_t prot) {
	kern_return_t kr = mach_vm_protect(task, address, size, false, prot);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to protect memory: %s", mach_error_string(kr));
		return false;
	}
	return true;
}

bool memory_write_const(mach_port_t task, mach_vm_address_t address, void *buffer, mach_vm_size_t size) {
	if (memory_protect(task, address, size, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY)) {
		_scope({ memory_protect(task, address, size, VM_PROT_READ | VM_PROT_COPY); });
		return memory_write(task, address, buffer, size);
	}
	return false;
}

bool memory_write_ptr(mach_port_t task, mach_vm_address_t address, mach_vm_address_t val) {
	return memory_write(task, address, (void *)&val, sizeof(val));
}

bool memory_write_32(mach_port_t task, mach_vm_address_t address, uint32_t val) {
	return memory_write(task, address, (void *)&val, sizeof(val));
}

bool memory_write_16(mach_port_t task, mach_vm_address_t address, uint16_t val) {
	return memory_write(task, address, (void *)&val, sizeof(val));
}

bool memory_write_8(mach_port_t task, mach_vm_address_t address, uint8_t val) {
	return memory_write(task, address, (void *)&val, sizeof(val));
}

mach_vm_address_t memory_create(task_t task, void *buffer, size_t size) {
	auto address = memory_allocate(task, size);
	if (address != 0) {
		if (memory_write(task, address, buffer, size)) {
			return address;
		}
		memory_free(task, address, size);
	}
	return 0;
}

mach_vm_address_t kport_find(mach_vm_address_t proc_kptr, mach_port_t port) {
	auto task_kptr = memory_read_ptr(tfp0, proc_kptr + koffsets.proc_task);
	auto itk_space_kptr = memory_read_ptr(tfp0, task_kptr + koffsets.task_itk_space);
	auto is_table_kptr = memory_read_ptr(tfp0, itk_space_kptr + koffsets.ipc_space_is_table);
	auto port_kptr = memory_read_ptr(tfp0, is_table_kptr + (MACH_PORT_INDEX(port) * koffsets.size_ipc_entry));
	return port_kptr;
}

kern_return_t kset_special_port(host_priv_t host_priv, int which, mach_port_t port) {
	auto proc_kptr = current_proc;
	auto host_priv_kptr = kport_find(proc_kptr, host_priv);
	auto port_kptr = kport_find(proc_kptr, port);
	auto realhost_kptr = memory_read_ptr(tfp0, host_priv_kptr + koffsets.ipc_port_ip_kobject);
	auto slot_kptr = realhost_kptr + koffsets.host_special + (which * sizeof(mach_vm_address_t));
	memory_write_ptr(tfp0, slot_kptr, port_kptr);
	return KERN_SUCCESS;
}

mach_vm_address_t kvnode_find(int fd) {
	auto fdp = memory_read_ptr(tfp0, current_proc + koffsets.proc_p_fd);
	auto ofp = memory_read_ptr(tfp0, fdp + koffsets.filedesc_fd_ofiles);
	auto fpp = memory_read_ptr(tfp0, ofp + (fd * sizeof(mach_vm_address_t)));
	auto fgp = memory_read_ptr(tfp0, fpp + koffsets.fileproc_f_fglob);
	auto vnode = memory_read_ptr(tfp0, fgp + koffsets.fileglob_fg_data);
	return vnode;
}

bool kproc_fixup(pid_t pid) {
	task_t task = TASK_NULL;
	kern_return_t kr = task_for_pid(mach_task_self(), pid, &task);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to get task name port right for pid %d: %s", pid, mach_error_string(kr));
		return false;
	}
	_scope({ mach_port_deallocate(mach_task_self(), task); });
	auto task_port_kptr = kport_find(current_proc, task);
	auto task_kptr = memory_read_ptr(tfp0, task_port_kptr + koffsets.ipc_port_ip_kobject);
	auto proc_kptr = memory_read_ptr(tfp0, task_kptr + koffsets.task_bsd_info);
	char path[PROC_PIDPATHINFO_MAXSIZE] = {0};
	struct stat st = {0};
	if (proc_pidpath(pid, path, sizeof(path)) >= 0 && lstat(path, &st) == 0 && (st.st_mode & (S_ISUID | S_ISGID)) != 0) {
		auto ucred_kptr = memory_read_ptr(tfp0, proc_kptr + koffsets.proc_p_ucred);
		if ((st.st_mode & S_ISUID) != 0) {
			memory_write_32(tfp0, proc_kptr + koffsets.proc_p_svuid, st.st_uid);
			memory_write_32(tfp0, ucred_kptr + koffsets.ucred_cr_svuid, st.st_uid);
			memory_write_32(tfp0, ucred_kptr + koffsets.ucred_cr_uid, st.st_uid);
		}
		if ((st.st_mode & S_ISGID) != 0) {
			memory_write_32(tfp0, proc_kptr + koffsets.proc_p_svgid, st.st_gid);
			memory_write_32(tfp0, ucred_kptr + koffsets.ucred_cr_svgid, st.st_gid);
			memory_write_32(tfp0, ucred_kptr + koffsets.ucred_cr_groups, st.st_gid);
		}
	}
	uint32_t proc_flag = memory_read_32(tfp0, proc_kptr + koffsets.proc_p_flag);
	if ((proc_flag & P_SUGID) != 0) {
		proc_flag &= ~P_SUGID;
		memory_write_32(tfp0, proc_kptr + koffsets.proc_p_flag, proc_flag);
	}
	uint32_t csflags = memory_read_32(tfp0, proc_kptr + koffsets.proc_p_csflags);
	csflags |= (CS_PLATFORM_BINARY | CS_VALID | CS_DEBUGGED | CS_SIGNED);
	csflags &= ~(CS_HARD | CS_KILL);
	memory_write_32(tfp0, proc_kptr + koffsets.proc_p_csflags, csflags);
	uint32_t task_flags = memory_read_32(tfp0, task_kptr + koffsets.task_t_flags);
	if ((task_flags & TF_PLATFORM) == 0) {
		task_flags |= TF_PLATFORM;
		memory_write_32(tfp0, task_kptr + koffsets.task_t_flags, task_flags);
	}
	return true;
}

void kcsblob_fixup(mach_vm_address_t kcsblob) {
	while (kcsblob != 0) {
		memory_write_32(tfp0, kcsblob + offsetof(struct cs_blob, csb_cpu_type), cpu_type);
		memory_write_32(tfp0, kcsblob + offsetof(struct cs_blob, csb_platform_binary), true);
		uint32_t csb_flags = memory_read_32(tfp0, kcsblob + offsetof(struct cs_blob, csb_flags));
		csb_flags |= (CS_SIGNED | CS_VALID | CS_PLATFORM_BINARY);
		memory_write_32(tfp0, kcsblob + offsetof(struct cs_blob, csb_reconstituted), true);
		memory_write_32(tfp0, kcsblob + offsetof(struct cs_blob, csb_flags), csb_flags);
		kcsblob = memory_read_ptr(tfp0, kcsblob + offsetof(struct cs_blob, csb_next));
	}
}

#define import_offset(name) \
	do { \
		if (!has_offset(#name)) { \
			error_log("Missing offset: %s", #name); \
			return false; \
		} \
		koffsets.name = (__typeof__(koffsets.name))get_offset(#name); \
	} while (false)

bool init_kernel_tooling(void) {
	kern_return_t kr;
	host_t host = mach_host_self();
	_scope({ mach_port_deallocate(mach_task_self(), host); });
	kr = host_get_special_port(host, HOST_LOCAL_NODE, 4, &tfp0);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to get kernel task port: %s", mach_error_string(kr));
		return false;
	}
	struct task_dyld_info dyld_info = {0};
	mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;
	kr = task_info(tfp0, TASK_DYLD_INFO, (task_info_t)&dyld_info, &count);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to get kernel task info: %s", mach_error_string(kr));
		return false;
	}
	offset_cache = dyld_info.all_image_info_addr;
	kernel_slide = dyld_info.all_image_info_size;
	uint32_t size = memory_read_32(tfp0, offset_cache + offsetof(struct cache_blob, size));
	struct cache_blob *blob = create_cache_blob(size);
	if (blob == NULL) {
		error_log("Unable to create cache blob");
		return false;
	}
	_scope({ free(blob); });
	if (!memory_read(tfp0, offset_cache, (void *)blob, (vm_size_t)size)) {
		error_log("Unable to read offset cache");
		return false;
	}
	import_cache_blob(blob);
	_scope({ destroy_cache(); });
	import_offset(allproc);
	import_offset(cs_blob_generation_count);
	import_offset(proc_p_pid);
	import_offset(proc_p_ucred);
	import_offset(proc_p_svuid);
	import_offset(proc_p_svgid);
	import_offset(proc_p_csflags);
	import_offset(proc_p_textvp);
	import_offset(proc_p_flag);
	import_offset(proc_p_fd);
	import_offset(proc_task);
	import_offset(filedesc_fd_ofiles);
	import_offset(fileproc_f_fglob);
	import_offset(fileglob_fg_data);
	import_offset(ucred_cr_svuid);
	import_offset(ucred_cr_uid);
	import_offset(ucred_cr_svgid);
	import_offset(ucred_cr_groups);
	import_offset(ucred_cr_label);
	import_offset(label_sandbox);
	import_offset(vnode_v_ubcinfo);
	import_offset(ubc_info_csblobs);
	import_offset(task_itk_space);
	import_offset(ipc_space_is_table);
	import_offset(size_ipc_entry);
	import_offset(host_special);
	import_offset(task_t_flags);
	import_offset(ipc_port_ip_kobject);
	import_offset(ipc_space_is_task);
	import_offset(task_bsd_info);
	koffsets.container_profile = (mach_vm_address_t)get_offset("container_profile");
	for (auto proc = memory_read_ptr(tfp0, koffsets.allproc); proc != 0; proc = memory_read_ptr(tfp0, proc)) {
		if (memory_read_32(tfp0, proc + koffsets.proc_p_pid) == getpid()) {
			current_proc = proc;
			break;
		}
	}
	if (current_proc == 0) {
		error_log("Unable to find our current process address");
		return false;
	}
	return true;
}

int ubc_cs_generation_check(mach_vm_address_t vp) {
	int retval = ENEEDAUTH;
	auto ubcinfo = memory_read_ptr(tfp0, vp + koffsets.vnode_v_ubcinfo);
	if (ubcinfo != 0 && memory_read_32(tfp0, ubcinfo + offsetof(struct ubc_info, cs_add_gen)) == memory_read_32(tfp0, koffsets.cs_blob_generation_count)) {
		retval = 0;
	}
	return retval;
}

void cs_blob_reset_cache(void) {
	memory_write_32(tfp0, koffsets.cs_blob_generation_count, memory_read_32(tfp0, koffsets.cs_blob_generation_count) + 2);
}

kern_return_t substitute_setup_process(mach_port_t serverPort, pid_t target_pid, bool set_exec, bool should_resume, audit_token_t token) {
	dispatch_async(our_queue, ^{
		bool shouldContinue = false;
		for (int i = 0; i < 1000; i++) {
			struct proc_bsdinfo proc;
			proc.pbi_status = 0;
			if (proc_pidinfo(target_pid, PROC_PIDTBSDINFO, 0, (void *)&proc, PROC_PIDTBSDINFO_SIZE) == -1) {
				error_log("Unable to get process info with pid %d: %s", target_pid, strerror(errno));
				break;
			}
			if (!set_exec || proc.pbi_status == SSTOP) {
				shouldContinue = true;
				break;
			}
			usleep(1000);
		}
		if (!shouldContinue) {
			error_log("Unable to set up process with pid: %d", target_pid);
			return;
		}
		debug_log("Setting up process with pid: %d", target_pid);
		kproc_fixup(target_pid);
		if (should_resume) {
			debug_log("Resuming process with pid: %d", target_pid);
			kill(target_pid, SIGCONT);
		}
	});
	return KERN_SUCCESS;
}

kern_return_t substitute_setup_self(mach_port_t serverPort, audit_token_t token) {
	pid_t pid = token.val[5];
	debug_log("Setting up process with pid: %d", pid);
	kproc_fixup(pid);
	if (pid == 1) {
		cs_blob_reset_cache();
	}
	return KERN_SUCCESS;
}

kern_return_t substitute_copy_extensions(mach_port_t serverPort, task_t task, mach_vm_address_t *stringAddress, mach_vm_size_t *stringSize, audit_token_t token) {
	_scope({ mach_port_deallocate(mach_task_self(), task); });
	if (sandbox_extensions == NULL) {
		return KERN_FAILURE;
	}
	*stringSize = (mach_vm_size_t)(strlen(sandbox_extensions) + 1);
	*stringAddress = memory_create(task, sandbox_extensions, *stringSize);
	if (*stringAddress == 0) {
		return KERN_FAILURE;
	}
	return KERN_SUCCESS;
}

kern_return_t add_blob_internal(const char *path, CS_SuperBlob *blob, size_t blobSize, uint64_t file_offset, cdhash_t realCDHash, cdhash_t fakeCDHash, void *context) {
	if (path == NULL || blob == NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		error_log("open: %s", strerror(errno));
		return KERN_FAILURE;
	}
	_scope({ close(fd); });
	flock(fd, LOCK_EX);
	_scope({ flock(fd, LOCK_UN); });
	auto vnode = kvnode_find(fd);
	if (vnode == 0) {
		error_log("Unable to find vnode: %s", path);
		return KERN_FAILURE;
	}
	if (ubc_cs_generation_check(vnode) != ENEEDAUTH) {
		return KERN_SUCCESS;
	}
	auto ubcinfo = memory_read_ptr(tfp0, vnode + koffsets.vnode_v_ubcinfo);
	if (ubcinfo == 0) {
		error_log("Unable to find ubcinfo: %s", path);
		return KERN_FAILURE;
	}
	if (strcmp(path, ourPath) == 0) {
		return KERN_SUCCESS;
	}
	auto csblobs = memory_read_ptr(tfp0, ubcinfo + koffsets.ubc_info_csblobs);
	if (csblobs != 0) {
		char new_path[] = "/Library/Caches/sub.XXXXXX";
		mktemp(new_path);
		int new_fd = open(new_path, O_RDONLY | O_CREAT | O_TRUNC, 0644);
		if (new_fd == -1) {
			error_log("open: %s", strerror(errno));
			return KERN_FAILURE;
		}
		_scope({ close(new_fd); });
		_scope({ unlink(new_path); });
		auto new_vnode = kvnode_find(new_fd);
		if (new_vnode == 0) {
			error_log("Unable to get new vnode: %s", path);
			return KERN_FAILURE;
		}
		auto new_ubcinfo = memory_read_ptr(tfp0, new_vnode + koffsets.vnode_v_ubcinfo);
		if (new_ubcinfo == 0) {
			error_log("Unable to get new ubcinfo: %s", path);
			return KERN_FAILURE;
		}
		memory_write_ptr(tfp0, new_ubcinfo + koffsets.ubc_info_csblobs, csblobs);
		csblobs = 0;
		memory_write_ptr(tfp0, ubcinfo + koffsets.ubc_info_csblobs, csblobs);
	}
	fsignatures_t signature;
	signature.fs_file_start = file_offset;
	signature.fs_blob_start = (void *)blob;
	signature.fs_blob_size = blobSize;
	if (!cdhash_hooks_add(realCDHash, fakeCDHash)) {
		error_log("Unable to hook cdhash: %s", path);
		return KERN_FAILURE;
	}
	_scope({ cdhash_hook_remove(realCDHash); });
	if (fcntl(fd, F_ADDSIGS, &signature) == -1) {
		error_log("fcntl: %s", strerror(errno));
		return KERN_FAILURE;
	}
	csblobs = memory_read_ptr(tfp0, ubcinfo + koffsets.ubc_info_csblobs);
	if (csblobs == 0) {
		error_log("Unable to find csblobs: %s", path);
		return KERN_FAILURE;
	}
	kcsblob_fixup(csblobs);
	return KERN_SUCCESS;
}

kern_return_t needs_blob_internal(const char *path, bool *needsBlob, void *context) {
	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		error_log("open: %s", strerror(errno));
		return KERN_FAILURE;
	}
	_scope({ close(fd); });
	flock(fd, LOCK_EX);
	_scope({ flock(fd, LOCK_UN); });
	auto vnode = kvnode_find(fd);
	if (vnode == 0) {
		error_log("Unable to find vnode: %s", path);
		return KERN_FAILURE;
	}
	if (ubc_cs_generation_check(vnode) == ENEEDAUTH) {
		*needsBlob = true;
	} else {
		*needsBlob = false;
	}
	return KERN_SUCCESS;
}

kern_return_t substitute_preflight(mach_port_t serverPort, task_t task, mach_vm_address_t pathAddress, mach_vm_size_t pathCnt, audit_token_t token) {
	_scope({ mach_port_deallocate(mach_task_self(), task); });
	if (pathCnt > PATH_MAX) {
		return KERN_NO_SPACE;
	}
	char *path = (char *)memory_copy_data(task, pathAddress, pathCnt);
	if (path == NULL) {
		return KERN_FAILURE;
	}
	_scope({ free(path); });
	if (path[pathCnt - 1] != '\0') {
		return KERN_FAILURE;
	}
	load_code_signatures(path, NULL);
	return KERN_SUCCESS;
}

kern_return_t verify_code_directory(mach_port_t amfid_port, amfid_path_t path, uint64_t file_offset, int32_t a4, int32_t a5, int32_t a6, int32_t *entitlements_valid, int32_t *signature_valid, int32_t *unrestrict, int32_t *signer_type, int32_t *is_apple, int32_t *is_developer_code, amfid_a13_t a13,
									amfid_cdhash_t cdhash, audit_token_t audit) {
	audit_token_t kernel_token = KERNEL_AUDIT_TOKEN_VALUE;
	if (memcmp(&audit, &kernel_token, sizeof(audit)) != 0) {
		return KERN_FAILURE;
	}
	if (!cs_forgery_get_cdhash(path, cdhash)) {
		return KERN_FAILURE;
	}
	cdhash_hook_get(cdhash, cdhash);
	*entitlements_valid = true;
	*signature_valid = true;
	*signer_type = false;
	*is_apple = false;
	*is_developer_code = false;
	const char *appContainers = "/private/var/containers/Bundle/Application";
	if (strncmp(path, appContainers, strlen(appContainers)) == 0) {
		*unrestrict = false;
	} else {
		*unrestrict = true;
	}
	return KERN_SUCCESS;
}

kern_return_t permit_unrestricted_debugging(mach_port_t amfid_port, int32_t *unrestricted_debugging, audit_token_t audit) {
	return KERN_FAILURE;
}

void set_cdhash(uint8_t cdhash[CS_CDHASH_LEN]) {
	auto ktextvp = memory_read_ptr(tfp0, current_proc + koffsets.proc_p_textvp);
	auto kubcinfo = memory_read_ptr(tfp0, ktextvp + koffsets.vnode_v_ubcinfo);
	auto kcsblobs = memory_read_ptr(tfp0, kubcinfo + koffsets.ubc_info_csblobs);
	while (kcsblobs != 0) {
		memory_write(tfp0, kcsblobs + offsetof(struct cs_blob, csb_cdhash), cdhash, CS_CDHASH_LEN);
		kcsblobs = memory_read_ptr(tfp0, kcsblobs);
	}
}

bool patch_amfid(void) {
	if (csops(getpid(), CS_OPS_CDHASH, real_cdhash, CS_CDHASH_LEN) == -1) {
		error_log("Unable to get our own cdhash: %s", strerror(errno));
		return false;
	}
	if (!cs_forgery_get_cdhash("/usr/libexec/amfid", fake_cdhash)) {
		error_log("Unable to get amfid's cdhash");
		return false;
	}
	set_cdhash(fake_cdhash);
	forged_cdhash = true;
	kern_return_t kr;
	host_t host = mach_host_self();
	_scope({ mach_port_deallocate(mach_task_self(), host); });
	kr = host_get_special_port(host, HOST_LOCAL_NODE, HOST_AMFID_PORT, &amfid_port);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to get amfid port: %s", mach_error_string(kr));
		return false;
	}
	mach_port_options_t options;
	options.flags = MPO_INSERT_SEND_RIGHT;
	kr = mach_port_construct(mach_task_self(), &options, 0, &fake_amfid_port);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to construct fake amfid port: %s", mach_error_string(kr));
		return false;
	}
	kr = kset_special_port(host, HOST_AMFID_PORT, fake_amfid_port);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to set fake amfid port: %s", mach_error_string(kr));
		return false;
	}
	dispatch_queue_attr_t attr = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_CONCURRENT, QOS_CLASS_USER_INTERACTIVE, -1);
	amfid_queue = dispatch_queue_create("science.xnu.MobileFileIntegrity", attr);
	amfid_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_MACH_RECV, fake_amfid_port, 0, amfid_queue);
	dispatch_source_set_event_handler(amfid_source, ^{
		dispatch_mig_server(amfid_source, 4184, amfid_server);
	});
	dispatch_resume(amfid_source);
	return true;
}

void unpatch_amfid(void) {
	if (forged_cdhash) {
		set_cdhash(real_cdhash);
		forged_cdhash = false;
	}
	amfid_source = NULL;
	amfid_queue = NULL;
	if (MACH_PORT_VALID(amfid_port)) {
		host_t host = mach_host_self();
		_scope({ mach_port_deallocate(mach_task_self(), host); });
		kset_special_port(host, HOST_AMFID_PORT, amfid_port);
		mach_port_deallocate(mach_task_self(), amfid_port);
		amfid_port = MACH_PORT_NULL;
	}
	if (MACH_PORT_VALID(fake_amfid_port)) {
		mach_port_destruct(mach_task_self(), fake_amfid_port, 0, 0);
		fake_amfid_port = MACH_PORT_NULL;
	}
}

bool init_server(void) {
	kern_return_t kr;
	mach_port_options_t options;
	options.flags = MPO_INSERT_SEND_RIGHT;
	kr = mach_port_construct(mach_task_self(), &options, 0, &our_port);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to construct our server port: %s", mach_error_string(kr));
		return false;
	}
	host_t host = mach_host_self();
	_scope({ mach_port_deallocate(mach_task_self(), host); });
	kr = kset_special_port(host, HOST_KEXTD_PORT, our_port);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to set our server port: %s", mach_error_string(kr));
		return false;
	}
	dispatch_queue_attr_t attr = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_CONCURRENT, QOS_CLASS_USER_INTERACTIVE, -2);
	our_queue = dispatch_queue_create("science.xnu.substituted", attr);
	our_source = dispatch_source_create(DISPATCH_SOURCE_TYPE_MACH_RECV, our_port, 0, our_queue);
	dispatch_source_set_event_handler(our_source, ^{
		dispatch_mig_server(our_source, 0x1000, substitute_daemon_server);
	});
	dispatch_resume(our_source);
	return true;
}

void deinit_server(void) {
	our_source = NULL;
	our_queue = NULL;
	if (MACH_PORT_VALID(our_port)) {
		mach_port_destruct(mach_task_self(), our_port, 0, 0);
		our_port = MACH_PORT_NULL;
	}
}

void signal_handler(int signum) {
	unpatch_amfid();
	deinit_server();
	struct sigaction act = {.sa_handler = SIG_DFL};
	sigemptyset(&act.sa_mask);
	sigaction(signum, &act, NULL);
	raise(signum);
}

void install_sig_handler(void) {
	const int signals[] = {
		SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGEMT, SIGFPE, SIGBUS, SIGSEGV, SIGSYS, SIGPIPE, SIGALRM, SIGTERM, SIGXCPU, SIGXFSZ, SIGVTALRM, SIGPROF, SIGUSR1, SIGUSR2,
	};
	struct sigaction act = {.sa_handler = signal_handler};
	sigemptyset(&act.sa_mask);
	for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); i++) {
		int err = sigaction(signals[i], &act, NULL);
		if (err != 0) {
			panic_log("Unable to register handler for signal: %d", signals[i]);
		}
	}
}

cpu_type_t get_cpu_type(void) {
	cpu_type_t cpu_type = 0;
	size_t size = sizeof(cpu_type);
	if (sysctlbyname("hw.cputype", &cpu_type, &size, NULL, 0) != 0) {
		error_log("Unable to get cpu type: %s", strerror(errno));
		abort();
	}
	return cpu_type;
}

void add_file_extension(const char *ext, const char *path) {
	char *extension = sandbox_extension_issue_file(ext, path, 0, 0);
	if (extension == NULL) {
		panic_log("Unable to issue file extension \"%s\" for \"%s\"", ext, path);
	}
	if (sandbox_extensions == NULL) {
		sandbox_extensions = extension;
	} else {
		size_t new_size = strlen(sandbox_extensions) + strlen(":") + strlen(extension) + 1;
		sandbox_extensions = (char *)reallocf(sandbox_extensions, new_size);
		if (sandbox_extensions == NULL) {
			panic_log("Unable to reallocate sandbox extensions: %s", strerror(errno));
		}
		strcat(sandbox_extensions, ":");
		strcat(sandbox_extensions, extension);
		sandbox_extensions[new_size - 1] = '\0';
		free(extension);
	}
}

void add_file_executable_extension(const char *path) {
	return add_file_extension("com.apple.sandbox.executable", path);
}

void add_file_read_extension(const char *path) {
	return add_file_extension("com.apple.app-sandbox.read", path);
}

void add_file_read_write_extension(const char *path) {
	return add_file_extension("com.apple.app-sandbox.read-write", path);
}

size_t remote_strlen(task_t task, mach_vm_address_t s) {
	size_t len = 0;
	while (memory_read_16(task, s++) != '\0')
		len++;
	return len;
}

mach_vm_address_t find_image(task_t task, const char *name) {
	struct task_dyld_info info = {0};
	mach_msg_type_number_t cnt = TASK_DYLD_INFO_COUNT;
	kern_return_t kr = task_info(task, TASK_DYLD_INFO, (task_info_t)&info, &cnt);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to get dyld info: %s", mach_error_string(kr));
		return 0;
	}
	if (info.all_image_info_format != TASK_DYLD_ALL_IMAGE_INFO_64) {
		error_log("Unexpected all_image_info_format: %d", info.all_image_info_format);
		return 0;
	}
	auto infosPtr = info.all_image_info_addr;
	auto infoArrayPtr = memory_read_ptr(task, infosPtr + offsetof(struct dyld_all_image_infos, infoArray));
	uint32_t infoArrayCount = memory_read_32(task, infosPtr + offsetof(struct dyld_all_image_infos, infoArrayCount));
	for (uint32_t i = 0; i < infoArrayCount; i++) {
		auto infoPtr = infoArrayPtr + sizeof(struct dyld_image_info) * i;
		auto pathPtr = memory_read_ptr(task, infoPtr + offsetof(struct dyld_image_info, imageFilePath));
		size_t pathSize = remote_strlen(task, pathPtr) + 1;
		char *pathBuffer = (char *)memory_copy_data(task, pathPtr, pathSize);
		if (pathBuffer == NULL) {
			continue;
		}
		_scope({ free(pathBuffer); });
		if (pathBuffer[pathSize - 1] != '\0') {
			continue;
		}
		if (strcmp(pathBuffer, name) != 0) {
			continue;
		}
		auto headerPtr = memory_read_ptr(task, infoPtr + offsetof(struct dyld_image_info, imageLoadAddress));
		return headerPtr;
	}
	return 0;
}

mach_vm_offset_t find_image_vmaddr_slide(task_t task, mach_vm_address_t headerPtr) {
	auto cmds = headerPtr + sizeof(struct mach_header_64);
	uint32_t ncmds = memory_read_32(task, headerPtr + offsetof(struct mach_header_64, ncmds));
	auto lc = cmds;
	for (int i = 0; i < ncmds; i++) {
		uint32_t cmd = memory_read_32(task, lc + offsetof(struct load_command, cmd));
		if (cmd == LC_SEGMENT_64) {
			auto fsgp = lc;
			auto vmaddr = memory_read_ptr(task, fsgp + offsetof(struct segment_command_64, vmaddr));
			return headerPtr - vmaddr;
		}
		uint32_t cmdsize = memory_read_32(task, lc + offsetof(struct load_command, cmdsize));
		lc += cmdsize;
	}
	return -1;
}

mach_vm_address_t remote__NSGetEnviron(task_t task) {
	const char *libsystem_c = "/usr/lib/system/libsystem_c.dylib";
	struct substitute_image *libsystem_c_image = substitute_open_image(libsystem_c);
	if (libsystem_c_image == NULL) {
		error_log("Unable to open libsystem_c");
		return 0;
	}
	_scope({ substitute_close_image(libsystem_c_image); });

	const char *symbol_name = "_environ_pointer";
	void *symbol_ptr = NULL;
	substitute_find_private_syms(libsystem_c_image, &symbol_name, &symbol_ptr, 1);
	if (symbol_ptr == NULL) {
		error_log("Unable to find symbol_ptr");
		return 0;
	}
	debug_log("Found symbol_ptr: %p", symbol_ptr);

	auto remote_libsystem_c = find_image(task, libsystem_c);
	if (remote_libsystem_c == 0) {
		error_log("Unable to find remote_libsystem_c");
		return 0;
	}
	debug_log("Found remote_libsystem_c: %llx", remote_libsystem_c);

	mach_vm_offset_t remote_libsystem_c_slide = find_image_vmaddr_slide(task, remote_libsystem_c);
	if (remote_libsystem_c_slide == -1) {
		error_log("Unable to find remote_libsystem_c_slide");
		return 0;
	}
	debug_log("Found remote_libsystem_c_slide: %llx", remote_libsystem_c_slide);

	auto remote_symbol_ptr = (mach_vm_address_t)symbol_ptr - (mach_vm_offset_t)libsystem_c_image->slide + (mach_vm_offset_t)remote_libsystem_c_slide;
	debug_log("Found remote_symbol_ptr: %llx", remote_symbol_ptr);

	auto remote_environ_ptr = memory_read_ptr(task, remote_symbol_ptr);
	if (remote_environ_ptr == 0) {
		error_log("Unable to find remote_environ_ptr");
		return 0;
	}
	debug_log("Found remote_environ_ptr: %llx", remote_environ_ptr);

	return remote_environ_ptr;
}

int remote_setenv(task_t task, const char *name, const char *value, int overwrite) {
	auto remote_environ_ptr = remote__NSGetEnviron(task);
	if (remote_environ_ptr == 0) {
		return -1;
	}

	auto remote_environ = memory_read_ptr(task, remote_environ_ptr);
	if (remote_environ == 0) {
		error_log("Unable to find remote_environ");
		return -1;
	}
	debug_log("Found remote_environ: %llx", remote_environ);

	mach_vm_size_t remote_environ_size = 0;
	auto ptr = remote_environ;
	for (;;) {
		_scope({ remote_environ_size += sizeof(mach_vm_address_t); });

		auto string = memory_read_ptr(task, ptr);
		if (string == 0) {
			break;
		}
		_scope({ ptr += sizeof(ptr); });

		mach_vm_size_t size = remote_strlen(task, string) + 1;
		char *string_buf = (__typeof__(string_buf))memory_copy_data(task, string, size);
		if (string_buf == NULL) {
			continue;
		}
		_scope({ free(string_buf); });

		char *value_ptr = string_buf;
		char *name_ptr = strsep(&value_ptr, "=");
		if (name_ptr == NULL || value_ptr == NULL) {
			continue;
		}

		debug_log("Found %s=%s", name_ptr, value_ptr);

		if (strcmp(name, name_ptr) != 0) {
			continue;
		}

		if (!overwrite) {
			return 0;
		}

		if (strlen(value_ptr) < strlen(value)) {
			continue;
		}

		size_t new_string_size = strlen(name_ptr) + strlen("=") + strlen(value_ptr) + sizeof(char);
		char *new_string_buf = (char *)malloc(new_string_size);
		if (new_string_buf == NULL) {
			error_log("Unable to allocate memory for new_string_buf: %s", strerror(errno));
			return -1;
		}
		_scope({ free(new_string_buf); });

		strcpy(new_string_buf, name_ptr);
		strcat(new_string_buf, "=");
		strcat(new_string_buf, value_ptr);
		new_string_buf[new_string_size - 1] = '\0';

		if (!memory_write(task, string, (void *)new_string_buf, (mach_vm_size_t)new_string_size)) {
			error_log("Unable to write new_string_buf to string");
			return -1;
		}
		debug_log("Wrote new_string_buf to string");
		return 0;
	}

	debug_log("Found remote_environ_size: %llx", remote_environ_size);

	size_t string_size = strlen(name) + strlen("=") + strlen(value) + sizeof(char);
	char *string_buf = (char *)malloc(string_size);
	if (string_buf == NULL) {
		error_log("Unable to allocate memory for string_buf: %s", strerror(errno));
		return -1;
	}
	_scope({ free(string_buf); });

	strcpy(string_buf, name);
	strcat(string_buf, "=");
	strcat(string_buf, value);
	string_buf[string_size - 1] = '\0';

	auto string = memory_create(task, (void *)string_buf, (mach_vm_size_t)string_size);
	if (string == 0) {
		error_log("Unable to create memory for string: %s", strerror(errno));
		return -1;
	}

	mach_vm_size_t new_remote_environ_size = remote_environ_size + sizeof(mach_vm_address_t);
	auto new_remote_environ = memory_allocate(task, new_remote_environ_size);
	if (new_remote_environ == 0) {
		error_log("Unable to create new_remote_environ");
		return -1;
	}
	debug_log("Created new_remote_environ: %llx", new_remote_environ);

	if (!memory_copy(task, remote_environ, new_remote_environ, remote_environ_size)) {
		error_log("Unable to copy remote_environ to new_remote_environ");
		return -1;
	}
	debug_log("Copied remote_environ to new_remote_environ");

	memory_write_ptr(task, new_remote_environ + remote_environ_size - sizeof(mach_vm_address_t), string);
	memory_write_ptr(task, new_remote_environ + remote_environ_size, 0);

	if (!memory_write_ptr(task, remote_environ_ptr, new_remote_environ)) {
		error_log("Unable to write new_remote_environ to remote_environ_ptr");
		return -1;
	}
	debug_log("Wrote new_remote_environ to remote_environ_ptr");

	return 0;
}

bool patch_launchd(void) {
	task_t task = TASK_NULL;
	kern_return_t kr = task_for_pid(mach_task_self(), 1, &task);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to get task for launchd: %s", mach_error_string(kr));
		return false;
	}
	_scope({ mach_port_deallocate(mach_task_self(), task); });
	kr = task_set_special_port(task, TASK_SEATBELT_PORT, our_port);
	if (kr != KERN_SUCCESS) {
		error_log("Unable to set seatbelt special port: %s", mach_error_string(kr));
		return false;
	}
	if (remote_setenv(task, "DYLD_INSERT_LIBRARIES", INSERTER_PATH, true) == -1) {
		error_log("Unable to insert to launchd");
		return false;
	}
	// Enable get-task-allow
	cs_blob_reset_cache();
	load_code_signatures("/sbin/launchd", NULL);
	return true;
}

int main(int argc, char **argv, char **envp) {
	host_t host = mach_host_self();
	_scope({ mach_port_deallocate(mach_task_self(), host); });
	kern_return_t kr = host_get_kextd_port(host, &our_port);
	if (kr == KERN_SUCCESS && MACH_PORT_VALID(our_port)) {
		mach_port_deallocate(mach_task_self(), our_port);
		error_log("Already running");
		return EXIT_SUCCESS;
	}
	proc_pidpath(getpid(), ourPath, sizeof(ourPath));
	if (!init_kernel_tooling()) {
		error_log("Unable to initialize kernel tooling");
		return EXIT_FAILURE;
	}
	cpu_type = get_cpu_type();
	if (argc != 2 || strcmp(argv[1], "-d") != 0) {
		auto vnode = memory_read_ptr(tfp0, current_proc + koffsets.proc_p_textvp);
		auto ubcinfo = memory_read_ptr(tfp0, vnode + koffsets.vnode_v_ubcinfo);
		auto csblobs = memory_read_ptr(tfp0, ubcinfo + koffsets.ubc_info_csblobs);
		kcsblob_fixup(csblobs);
		info_log("Spawning server");
		sem_unlink(SEMAPHORE_NAME);
		sem_t *sem = sem_open(SEMAPHORE_NAME, O_CREAT | O_EXCL, 0644, 0);
		if (sem == SEM_FAILED) {
			error_log("Unable to create semaphore: %s", strerror(errno));
			return EXIT_FAILURE;
		}
		_scope({ sem_close(sem); });
		pid_t serverPid = 0;
		const char *serverArgs[] = {
			ourPath,
			"-d",
			NULL,
		};
		posix_spawnattr_t attr = NULL;
		posix_spawnattr_init(&attr);
		_scope({ posix_spawnattr_destroy(&attr); });
		posix_spawnattr_setflags(&attr, POSIX_SPAWN_CLOEXEC_DEFAULT);
		if (int ret = posix_spawn(&serverPid, ourPath, NULL, (const posix_spawnattr_t *)&attr, (char **)&serverArgs, NULL)) {
			error_log("Unable to spawn server: %s", strerror(ret));
			return EXIT_FAILURE;
		}
		__block int ret;
		dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
		dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
			ret = sem_wait(sem);
			dispatch_semaphore_signal(semaphore);
		});
		if (dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, NSEC_PER_SEC * 10)) != 0) {
			error_log("Waiting for our daemon timed out");
			sem_post(sem);
			return EXIT_FAILURE;
		} else if (ret == -1) {
			error_log("Unable to acquire semaphore: %s", strerror(errno));
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	} else {
		install_sig_handler();
		sem_t *sem = sem_open(SEMAPHORE_NAME, O_RDWR);
		if (sem == SEM_FAILED) {
			error_log("Unable to open semaphore: %s", strerror(errno));
			return EXIT_FAILURE;
		}
		init_cs_bypass(add_blob_internal, needs_blob_internal);
		add_file_executable_extension("/Library");
		add_file_executable_extension("/Applications");
		add_file_read_extension("/tmp");
		add_file_read_extension("/Applications");
		add_file_read_extension("/Library");
		add_file_read_extension("/System");
		add_file_read_extension("/private/var/mnt");
		add_file_read_extension("/private/var/mobile/Library");
		add_file_read_extension("/private/var/mobile/Documents");
		add_file_read_extension("/usr/sbin");
		add_file_read_extension("/usr/libexec");
		add_file_read_extension("/sbin");
		add_file_read_write_extension("/private/var/mobile/Library/Preferences/");
		add_file_read_write_extension("/private/var/log/extensionloader.log");
		if (!patch_amfid()) {
			error_log("Unable to patch amfid");
		} else if (!init_server()) {
			error_log("Unable to initialize server");
		} else if (!patch_launchd()) {
			error_log("Unable to patch launchd");
		} else {
			uint32_t p_flag = memory_read_32(tfp0, current_proc + koffsets.proc_p_flag);
			p_flag |= P_REBOOT;
			memory_write_32(tfp0, current_proc + koffsets.proc_p_flag, p_flag);
			info_log("Initialized successfully");
			sem_post(sem);
			dispatch_main();
		}
		unpatch_amfid();
		deinit_server();
		return EXIT_FAILURE;
	}
}
