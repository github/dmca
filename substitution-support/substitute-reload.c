#include <CoreFoundation/CoreFoundation.h>
#include <dirent.h>
#include <mach/mach.h>
#include <sys/stat.h>
#include <sysexits.h>
#include <xpc/xpc.h>

const char *xpc_strerror(int error);
bool xpc_dictionary_apply_f(xpc_object_t obj, void *func, void *context);
xpc_object_t xpc_pipe_create_from_port(mach_port_t port, int flags);
int xpc_pipe_routine(void *pipe, xpc_object_t message, xpc_object_t *reply);
#define OS_ALLOC_ONCE_KEY_LIBXPC 1
typedef long os_alloc_token_t;
struct _os_alloc_once_s {
	os_alloc_token_t once;
	void *ptr;
};
extern struct _os_alloc_once_s _os_alloc_once_table[];
void *_os_alloc_once(struct _os_alloc_once_s *slot, size_t sz, os_function_t init);
static void *os_alloc_once(os_alloc_token_t token, size_t sz, os_function_t init) {
	struct _os_alloc_once_s *slot = &_os_alloc_once_table[token];
	if (OS_EXPECT(slot->once, ~0l) != ~0l) {
		void *ptr = _os_alloc_once(slot, sz, init);
		OS_COMPILER_CAN_ASSUME(slot->once == ~0l);
		return ptr;
	}
	return slot->ptr;
}

static int send_command(uint64_t subsystem, uint64_t routine, xpc_object_t message, xpc_object_t *reply) {
	void *xpc_gd = os_alloc_once(OS_ALLOC_ONCE_KEY_LIBXPC, 0x1F8, 0);
	xpc_object_t xdict = NULL;
	xpc_dictionary_set_uint64(message, "subsystem", subsystem);
	xpc_dictionary_set_uint64(message, "routine", routine);
	if (!*((int8_t *)xpc_gd + 42))
		xpc_dictionary_set_bool(message, "pre-exec", 1);
	int rv = xpc_pipe_routine(*((void **)(xpc_gd + 0x18)), message, &xdict);
	if (!rv) {
		rv = xpc_dictionary_get_int64(xdict, "error");
		if (!(int32_t)rv) {
			*reply = xdict;
			rv = 0;
		}
	}
	return rv;
}

int main(int argc, char **argv) {
	if (getppid() != 1) {
		fprintf(stderr, "%s cannot be run directly.\n", getprogname());
		return EX_CONFIG;
	}

	remove("/Library/Caches/science.xnu.substitute.reload.plist");

	const char *dir_path = "/etc/rc.d";
	DIR *dir = opendir(dir_path);
	if (dir != NULL) {
		struct dirent *ent;
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
				continue;
			}
			char path[PATH_MAX];
			strcpy(path, dir_path);
			strcat(path, "/");
			strcat(path, ent->d_name);
			struct stat st;
			st.st_mode = 0;
			if (stat(path, &st) != 0 || (st.st_mode & S_IXUSR) == 0) {
				continue;
			}
			system(path);
		}
		closedir(dir);
	}

	xpc_object_t paths = xpc_array_create(0, 0);
	xpc_array_set_string(paths, XPC_ARRAY_APPEND, "continue-booting");
	if (access("/var/tmp/.substitute_disable_daemons", F_OK) != 0) {
		xpc_array_set_string(paths, XPC_ARRAY_APPEND, "/Library/LaunchDaemons");
	}

	xpc_object_t dict = xpc_dictionary_create(0, 0, 0);
	xpc_dictionary_set_uint64(dict, "type", 1);
	xpc_dictionary_set_uint64(dict, "handle", 0);
	xpc_dictionary_set_uint64(dict, "enable", 0);
	xpc_dictionary_set_value(dict, "paths", paths);

	xpc_object_t reply = 0;
	int ret = send_command(3, 800, dict, &reply);
	xpc_release(dict);
	if (ret != 0) {
		fprintf(stderr, "failed to message launchd!\n");
		return EX_SOFTWARE;
	}

	return EX_OK;
}