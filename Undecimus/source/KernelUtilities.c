#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <common.h>
#include <iokit.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <dirent.h>
#include <mach-o/fat.h>
#include <kerneldec.h>
#include <patchfinder64.h>
#include <sys/utsname.h>
#include <sandbox.h>
#include <libproc.h>
#include <fcntl.h>
#include <copyfile.h>
#include <spawn.h>
#import <CommonCrypto/CommonDigest.h>

#include "KernelMemory.h"
#include "KernelOffsets.h"
#include "KernelUtilities.h"
#if __has_include("find_port.h")
#include "find_port.h"
#else
#define find_port_address(port, disposition) KPTR_NULL
#endif
#include "KernelExecution.h"
#include "pac.h"
#include "kernel_call.h"
#include <cs_blobs.h>
#include <ubc_headers.h>
#include "insert_dylib.h"

#define OFFSET_CACHE_VERSION 0x13

#include "cs_dingling.h"

#define _assert(test) do { \
    if (test) break; \
    int saved_errno = errno; \
    LOG("%s(%d): Assertion failure", __FILENAME__, __LINE__); \
    errno = saved_errno; \
    goto out; \
} while(false)

#define P_MEMSTAT_INTERNAL 0x00001000 /* Process is a system-critical-not-be-jetsammed process i.e. launchd */

#define TF_PLATFORM 0x00000400 /* task is a platform binary */

#define IE_BITS_SEND (1<<16)
#define IE_BITS_RECEIVE (1<<17)

#define VSYSTEM 0x000004 /* vnode being used by kernel */
#define VNOFLUSH 0x040000 /* don't vflush() if SKIPSYSTEM */
#define VSHARED_DYLD 0x000200 /* vnode is a dyld shared cache file */
#define VISSHADOW 0x008000 /* vnode is a shadow file */

#define KMA_KOBJECT 0x04
#define VM_KERN_MEMORY_WAITQ 17
#define VM_KERN_MEMORY_SECURITY 23

#define P_REBOOT 0x00200000 /* Process called reboot() */
#define P_LIST_CHILDDRAINED 0x00000100
#define HAS_CORPSE_FOOTPRINT 0x800

#define pmap_cs_blob_limit 0x1ff8

#define _Applications "/Applications"
#define _LaunchDaemons "/System/Library/LaunchDaemons"

#define FILE_READ_EXC_KEY "com.apple.security.exception.files.absolute-path.read-only"
#define FILE_READ_WRITE_EXC_KEY "com.apple.security.exception.files.absolute-path.read-write"
#define MACH_LOOKUP_EXC_KEY "com.apple.security.exception.mach-lookup.global-name"
#define MACH_REGISTER_EXC_KEY "com.apple.security.exception.mach-register.global-name"

#define DEFAULT_ENTITLEMENTS \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" \
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\"\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" \
    "<plist version=\"1.0\">" \
    "<dict>" \
    "<key>platform-application</key>" \
    "<true/>" \
    "<key>com.apple.private.security.no-container</key>" \
    "<true/>" \
    "<key>get-task-allow</key>" \
    "<true/>" \
    "<key>com.apple.private.skip-library-validation</key>" \
    "<true/>" \
    "</dict>" \
    "</plist>" \

static const char *file_read_exceptions[] = {
    "/Library",
    "/System",
    "/private/var/mnt",
    NULL
};

static const char *file_read_write_exceptions[] = {
    "/private/var/mobile/Library",
    NULL
};

static const char *mach_lookup_exceptions[] = {
    "cy:com.saurik.substrated",
    "ch.ringwald.hidsupport.backboard",
    "com.rpetrich.rocketbootstrapd",
    "com.apple.BTLEAudioController.xpc",
    "com.apple.backboard.hid.services",
    "com.apple.commcenter.coretelephony.xpc",
    NULL
};

static const char *mach_register_exceptions[] = {
    "ch.ringwald.hidsupport.backboard",
    NULL
};

#define MEMORYSTATUS_CMD_SET_JETSAM_TASK_LIMIT 6
int memorystatus_control(uint32_t command, int32_t pid, uint32_t flags, void *buffer, size_t buffersize);

kptr_t kernel_base = KPTR_NULL;
kptr_t offset_options = KPTR_NULL;
kptr_t offset_features = KPTR_NULL;
bool found_offsets = false;
kptr_t cached_task_self_addr = KPTR_NULL;
kptr_t cached_proc_struct_addr = KPTR_NULL;
static bool weird_offsets = false;
bool no_kernel_execution = false;

#define find_port(port, disposition) (have_kmem_read() && found_offsets ? get_address_of_port(proc_struct_addr(), port) : find_port_address(port, disposition))

int copyin(const void *uaddr, kptr_t kaddr, size_t len) {
    int ret = -1;
    kptr_t copyin_kptr = KPTR_NULL;
    _assert(uaddr != NULL);
    _assert(KERN_POINTER_VALID(kaddr));
    _assert(len > 0);
    copyin_kptr = kernel_offset(copyin);
    _assert(KERN_POINTER_VALID(copyin_kptr));
    ret = (int)kexec(copyin_kptr, (kptr_t)uaddr, kaddr, (kptr_t)len, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

int copyout(const kptr_t kaddr, void *udaddr, size_t len) {
    int ret = -1;
    kptr_t copyout_kptr = KPTR_NULL;
    _assert(udaddr != NULL);
    _assert(KERN_POINTER_VALID(kaddr));
    _assert(len > 0);
    copyout_kptr = kernel_offset(copyout);
    _assert(KERN_POINTER_VALID(copyout_kptr));
    ret = (int)kexec(copyout_kptr, (kptr_t)kaddr, (kptr_t)udaddr, (kptr_t)len, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

kptr_t task_self_addr()
{
    init_function();
    kptr_t ret = KPTR_NULL;
    if (KERN_POINTER_VALID((ret = cached_task_self_addr))) goto out;
    cached_task_self_addr = find_port(mach_task_self(), MACH_MSG_TYPE_COPY_SEND);
out:;
    return cached_task_self_addr;
}

kptr_t current_thread()
{
    init_function();
    kptr_t ret = KPTR_NULL;
    thread_t thread = THREAD_NULL;
    thread = mach_thread_self();
    _assert(MACH_PORT_VALID(thread));
    kptr_t thread_port = find_port(thread, MACH_MSG_TYPE_COPY_SEND);
    _assert(KERN_POINTER_VALID(thread_port));
    kptr_t thread_addr = ReadKernel64(thread_port + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
    _assert(thread_addr);
    ret = thread_addr;
out:;
    if (MACH_PORT_VALID(thread)) mach_port_deallocate(mach_task_self(), thread); thread = THREAD_NULL;
    return ret;
}

kptr_t find_kernel_base(void) {
	kptr_t base_kptr = KPTR_NULL;
	io_service_t service = IO_OBJECT_NULL;
	mach_port_t client = MACH_PORT_NULL;
	service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IOSurfaceRoot"));
	_assert(MACH_PORT_VALID(service));
	_assert(IOServiceOpen(service, mach_task_self(), 0, &client) == KERN_SUCCESS);
	_assert(MACH_PORT_VALID(client));
	kptr_t proc_kptr = proc_struct_addr();
	_assert(KERN_POINTER_VALID(proc_kptr));
	kptr_t client_kptr = get_address_of_port(proc_kptr, client);
	_assert(KERN_POINTER_VALID(client_kptr));
	kptr_t object_kptr = ReadKernel64(client_kptr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
	_assert(KERN_POINTER_VALID(object_kptr));
	kptr_t vtable_kptr = ReadKernel64(object_kptr);
	if (vtable_kptr != KPTR_NULL)
		vtable_kptr = kernel_xpacd(vtable_kptr);
	_assert(KERN_POINTER_VALID(vtable_kptr));
	kptr_t func_kptr = ReadKernel64(vtable_kptr + (koffset(KVTABLE_OFFSET_GET_EXTERNAL_TRAP_FOR_INDEX) * sizeof(kptr_t)));
	if (func_kptr != KPTR_NULL)
		func_kptr = kernel_xpacd(func_kptr);
	_assert(KERN_POINTER_VALID(func_kptr));
	uint64_t kernel_header_offset = 0x4000;
	uint64_t kernel_slide_step = 0x1000;
	base_kptr = (func_kptr & ~(kernel_slide_step - 1)) + kernel_header_offset;
	uint32_t magic_value = 0;
	for (;;) {
		if (!rkbuffer(base_kptr, &magic_value, sizeof(magic_value))) {
			base_kptr = 0;
			break;
		}
		if (magic_value == MH_MAGIC_64) {
			break;
		}
		base_kptr -= kernel_slide_step;
	}
out:;
	if (MACH_PORT_VALID(service))
		IOServiceClose(service);
	service = IO_OBJECT_NULL;
	if (MACH_PORT_VALID(service))
		mach_port_deallocate(mach_task_self(), service);
	service = MACH_PORT_NULL;
	return base_kptr;
}

mach_port_t kernel_host_port(void) {
    init_function();
    mach_port_t ret = MACH_PORT_NULL;
    mach_port_t port = MACH_PORT_NULL;
    kptr_t port_kptr = KPTR_NULL;
    _assert(mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port) == KERN_SUCCESS);
    _assert(MACH_PORT_VALID(port));
    _assert(mach_port_insert_right(mach_task_self(), port, port, MACH_MSG_TYPE_MAKE_SEND) == KERN_SUCCESS);
    port_kptr = find_port(port, MACH_MSG_TYPE_COPY_SEND);
    _assert(KERN_POINTER_VALID(port_kptr));
    _assert(WriteKernel32(port_kptr + koffset(KSTRUCT_OFFSET_IPC_PORT_IO_BITS), IO_BITS_ACTIVE | IKOT_HOST_PRIV));
    _assert(WriteKernel64(port_kptr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_RECEIVER), _ipc_space_kernel));
    _assert(WriteKernel64(port_kptr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT), _realhost));
    ret = port;
    port = MACH_PORT_NULL;
out:;
    if (MACH_PORT_VALID(port)) mach_port_deallocate(mach_task_self(), port); port = MACH_PORT_NULL;
    return ret;
}

#undef find_port

bool iterate_proc_list(void (^handler)(kptr_t, pid_t, bool *)) {
    init_function();
    bool ret = false;
    _assert(handler != NULL);
    bool iterate = true;
    kptr_t proc = _allproc;
    while (KERN_POINTER_VALID(proc) && iterate) {
        pid_t pid = ReadKernel32(proc + koffset(KSTRUCT_OFFSET_PROC_PID));
        handler(proc, pid, &iterate);
        if (!iterate) break;
        proc = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_P_LIST));
    }
    ret = true;
out:;
    return ret;
}

kptr_t get_proc_struct_for_pid(pid_t pid)
{
    init_function();
    __block kptr_t proc = KPTR_NULL;
    void (^handler)(kptr_t, pid_t, bool *) = ^(kptr_t found_proc, pid_t found_pid, bool *iterate) {
        if (found_pid == pid) {
            proc = found_proc;
            *iterate = false;
        }
    };
    _assert(iterate_proc_list(handler));
out:;
    return proc;
}

kptr_t proc_struct_addr()
{
    init_function();
    kptr_t ret = KPTR_NULL;
    if (KERN_POINTER_VALID((ret = cached_proc_struct_addr))) goto out;
    cached_proc_struct_addr = get_proc_struct_for_pid(getpid());
out:;
    return cached_proc_struct_addr;
}

kptr_t get_address_of_port(kptr_t proc, mach_port_t port)
{
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(proc));
    _assert(MACH_PORT_VALID(port));
    kptr_t task_addr = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_TASK));
    _assert(KERN_POINTER_VALID(task_addr));
    kptr_t itk_space = ReadKernel64(task_addr + koffset(KSTRUCT_OFFSET_TASK_ITK_SPACE));
    _assert(KERN_POINTER_VALID(itk_space));
    kptr_t is_table = ReadKernel64(itk_space + koffset(KSTRUCT_OFFSET_IPC_SPACE_IS_TABLE));
    _assert(KERN_POINTER_VALID(is_table));
    kptr_t port_addr = ReadKernel64(is_table + (MACH_PORT_INDEX(port) * koffset(KSTRUCT_SIZE_IPC_ENTRY)));
    _assert(KERN_POINTER_VALID(port_addr));
    ret = port_addr;
out:;
    return ret;
}

kptr_t get_kernel_cred_addr()
{
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t kernel_proc_struct_addr = _kernproc;
    _assert(KERN_POINTER_VALID(kernel_proc_struct_addr));
    kptr_t kernel_ucred_struct_addr = ReadKernel64(kernel_proc_struct_addr + koffset(KSTRUCT_OFFSET_PROC_UCRED));
    _assert(KERN_POINTER_VALID(kernel_ucred_struct_addr));
    ret = kernel_ucred_struct_addr;
out:;
    return ret;
}

kptr_t give_creds_to_process_at_addr(kptr_t proc, kptr_t cred_addr)
{
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(proc));
    _assert(KERN_POINTER_VALID(cred_addr));
    kptr_t proc_cred_addr = proc + koffset(KSTRUCT_OFFSET_PROC_UCRED);
    kptr_t current_cred_addr = ReadKernel64(proc_cred_addr);
    _assert(KERN_POINTER_VALID(current_cred_addr));
    _assert(WriteKernel64(proc_cred_addr, cred_addr));
    ret = current_cred_addr;
out:;
    return ret;
}

bool set_platform_binary(kptr_t proc, bool set)
{
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    kptr_t task_struct_addr = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_TASK));
    _assert(KERN_POINTER_VALID(task_struct_addr));
    kptr_t task_t_flags_addr = task_struct_addr + koffset(KSTRUCT_OFFSET_TASK_TFLAGS);
    uint32_t task_t_flags = ReadKernel32(task_t_flags_addr);
    if (set) {
        task_t_flags |= TF_PLATFORM;
    } else {
        task_t_flags &= ~(TF_PLATFORM);
    }
    _assert(WriteKernel32(task_struct_addr + koffset(KSTRUCT_OFFSET_TASK_TFLAGS), task_t_flags));
    ret = true;
out:;
    return ret;
}

kptr_t zm_fix_addr(kptr_t addr) {
    init_function();
    typedef struct {
        uint64_t prev;
        uint64_t next;
        uint64_t start;
        uint64_t end;
    } kmap_hdr_t;
    kptr_t zm_fixed_addr = KPTR_NULL;
    kmap_hdr_t *zm_hdr = NULL;
    zm_hdr = malloc(sizeof(kmap_hdr_t));
    _assert(zm_hdr != NULL);
    _assert(rkbuffer(_zone_map + 0x10, zm_hdr, sizeof(kmap_hdr_t)));
    _assert(zm_hdr->end - zm_hdr->start <= 0x100000000);
    kptr_t zm_tmp = (zm_hdr->start & 0xffffffff00000000) | ((addr) & 0xffffffff);
    zm_fixed_addr = zm_tmp < zm_hdr->start ? zm_tmp + 0x100000000 : zm_tmp;
out:;
    SafeFreeNULL(zm_hdr);
    return zm_fixed_addr;
}

bool verify_tfp0() {
    init_function();
    bool ret = false;
    size_t test_kptr_size = 0;
    kptr_t test_kptr = KPTR_NULL;
    kptr_t test_data = 0x4141414141414141;
    test_kptr_size = sizeof(kptr_t);
    test_kptr = kmem_alloc(test_kptr_size);
    _assert(KERN_POINTER_VALID(test_kptr));
    _assert(WriteKernel64(test_kptr, test_data));
    _assert(ReadKernel64(test_kptr) == test_data);
    ret = true;
out:;
    if (KERN_POINTER_VALID(test_kptr)) kmem_free(test_kptr, test_kptr_size); test_kptr = KPTR_NULL;
    return ret;
}

int (*pmap_load_trust_cache)(kptr_t kernel_trust, size_t length) = NULL;
int _pmap_load_trust_cache(kptr_t kernel_trust, size_t length) {
    init_function();
    int ret = -1;
    _assert(KERN_POINTER_VALID(kernel_trust));
    kptr_t function = kernel_offset(pmap_load_trust_cache);
    _assert(KERN_POINTER_VALID(function));
    ret = (int)kexec(function, kernel_trust, (kptr_t)length, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

bool set_host_type(host_t host, uint32_t type) {
    init_function();
    bool ret = false;
    _assert(MACH_PORT_VALID(host));
    kptr_t hostport_addr = get_address_of_port(proc_struct_addr(), host);
    _assert(KERN_POINTER_VALID(hostport_addr));
    _assert(WriteKernel32(hostport_addr, type));
    ret = true;
out:;
    return ret;
}

bool export_tfp0(host_t host) {
    init_function();
    bool ret = false;
    _assert(MACH_PORT_VALID(host));
    uint32_t type = IO_BITS_ACTIVE | IKOT_HOST_PRIV;
    _assert(set_host_type(host, type));
    ret = true;
out:;
    return ret;
}

bool unexport_tfp0(host_t host) {
    init_function();
    bool ret = false;
    _assert(MACH_PORT_VALID(host));
    uint32_t type = IO_BITS_ACTIVE | IKOT_HOST;
    _assert(set_host_type(host, type));
    ret = true;
out:;
    return ret;
}

bool set_csflags(kptr_t proc, uint32_t flags, bool value) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    kptr_t proc_csflags_addr = proc + koffset(KSTRUCT_OFFSET_PROC_P_CSFLAGS);
    uint32_t csflags = ReadKernel32(proc_csflags_addr);
    if (value == true) {
        csflags |= flags;
    } else {
        csflags &= ~flags;
    }
    _assert(WriteKernel32(proc_csflags_addr, csflags));
    ret = true;
out:;
    return ret;
}

bool set_csb_platform_binary(kptr_t proc, bool value) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    kptr_t textvp = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_TEXTVP));
    kptr_t ubcinfo = ReadKernel64(textvp + koffset(KSTRUCT_OFFSET_VNODE_V_UBCINFO));
    kptr_t csblobs = ReadKernel64(ubcinfo + koffset(KSTRUCT_OFFSET_UBC_INFO_CSBLOBS));
    while (KERN_POINTER_VALID(csblobs)) {
        WriteKernel32(csblobs + offsetof(struct cs_blob, csb_platform_binary), value);
        csblobs = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_next));
    }
    ret = true;
out:;
    return ret;
}

bool set_cs_platform_binary(kptr_t proc, bool value) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    _assert(set_csflags(proc, CS_PLATFORM_BINARY, value));
    ret = true;
out:;
    return ret;
}

bool set_p_flag(kptr_t proc, uint32_t flag, bool value) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    kptr_t proc_p_flag_addr = proc + koffset(KSTRUCT_OFFSET_PROC_P_FLAG);
    uint32_t p_flag = ReadKernel32(proc_p_flag_addr);
    if (value == true) {
        p_flag |= flag;
    } else {
        p_flag &= ~flag;
    }
    _assert(WriteKernel32(proc_p_flag_addr, p_flag));
    ret = true;
out:;
    return ret;
}

bool set_child_drained(kptr_t proc, bool value) {
    return set_p_flag(proc, P_LIST_CHILDDRAINED, value);
}

bool set_p_reboot(kptr_t proc, bool value) {
    return set_p_flag(proc, P_REBOOT, value);
}

bool kernel_set_signals_enabled(pid_t pid, bool enabled) {
    kptr_t proc = get_proc_struct_for_pid(pid);
    return set_p_reboot(proc, !enabled);
}

bool set_vm_map_flags(kptr_t proc, uint32_t flags, bool value) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    kptr_t task = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_TASK));
    _assert(KERN_POINTER_VALID(task));
    kptr_t vm_map = ReadKernel64(task + koffset(KSTRUCT_OFFSET_TASK_VM_MAP));
    _assert(KERN_POINTER_VALID(vm_map));
    kptr_t vm_map_flags_addr = vm_map + koffset(KSTRUCT_OFFSET_VM_MAP_FLAGS);
    uint32_t vm_map_flags = ReadKernel32(vm_map_flags_addr);
    if (value == true) {
        vm_map_flags |= flags;
    } else {
        vm_map_flags &= ~flags;
    }
    _assert(WriteKernel32(vm_map_flags_addr, vm_map_flags));
    ret = true;
out:;
    return ret;
}

bool set_has_corpse_footprint(kptr_t proc, bool value) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    _assert(set_vm_map_flags(proc, HAS_CORPSE_FOOTPRINT, value));
    ret = true;
out:;
    return ret;
}

bool execute_with_credentials(kptr_t proc, kptr_t credentials, void (^function)(void)) {
    init_function();
    bool ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(proc));
    _assert(KERN_POINTER_VALID(credentials));
    _assert(function != NULL);
    kptr_t saved_credentials = give_creds_to_process_at_addr(proc, credentials);
    _assert(KERN_POINTER_VALID(saved_credentials));
    function();
    ret = give_creds_to_process_at_addr(proc, saved_credentials);
out:;
    return ret;
}

uint32_t get_proc_memstat_state(kptr_t proc) {
    init_function();
    uint32_t ret = 0;
    _assert(KERN_POINTER_VALID(proc));
    uint32_t p_memstat_state = ReadKernel32(proc + koffset(KSTRUCT_OFFSET_PROC_P_MEMSTAT_STATE));
    ret = p_memstat_state;
out:;
    return ret;
}

bool set_proc_memstat_state(kptr_t proc, uint32_t memstat_state) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    _assert(WriteKernel32(proc + koffset(KSTRUCT_OFFSET_PROC_P_MEMSTAT_STATE), memstat_state));
    ret = true;
out:;
    return ret;
}

bool set_proc_memstat_internal(kptr_t proc, bool set) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    uint32_t memstat_state = get_proc_memstat_state(proc);
    if (set) {
        memstat_state |= P_MEMSTAT_INTERNAL;
    } else {
        memstat_state &= ~P_MEMSTAT_INTERNAL;
    }
    _assert(set_proc_memstat_state(proc, memstat_state));
    ret = true;
out:;
    return ret;
}

bool get_proc_memstat_internal(kptr_t proc) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(proc));
    uint32_t p_memstat_state = get_proc_memstat_state(proc);
    ret = (p_memstat_state & P_MEMSTAT_INTERNAL);
out:;
    return ret;
}

size_t kstrlen(kptr_t ptr) {
    init_function();
    size_t size = 0;
    _assert(KERN_POINTER_VALID(ptr));
    while (ReadKernel16(ptr++) != '\0') size++;;
out:;
    return size;
}

kptr_t sstrdup(const char *str) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t kstr = KPTR_NULL;
    size_t kstr_size = 0;
    _assert(str != NULL);
    kptr_t function = kernel_offset(sstrdup);
    _assert(KERN_POINTER_VALID(function));
    kstr_size = strlen(str) + 1;
    kstr = kmem_alloc(kstr_size);
    _assert(KERN_POINTER_VALID(kstr));
    _assert(wkbuffer(kstr, (void *)str, kstr_size));
    ret = kstr;
out:;
    return ret;
}

kptr_t smalloc(size_t size) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t function = kernel_offset(smalloc);
    _assert(KERN_POINTER_VALID(function));
    ret = kexec(function, (kptr_t)size, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    if (ret != KPTR_NULL) ret = zm_fix_addr(ret);
out:;
    return ret;
}

void sfree(kptr_t ptr) {
    init_function();
    _assert(KERN_POINTER_VALID(ptr));
    kptr_t function = kernel_offset(sfree);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, ptr, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}


kptr_t IOMalloc(vm_size_t size) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t function = kernel_offset(IOMalloc);
    _assert(KERN_POINTER_VALID(function));
    ret = kexec(function, (kptr_t)size, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    if (ret != KPTR_NULL) ret = zm_fix_addr(ret);
out:;
    return ret;
}

void IOFree(kptr_t address, vm_size_t size) {
    init_function();
    _assert(KERN_POINTER_VALID(address));
    _assert(size > 0);
    kptr_t function = kernel_offset(IOFree);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, address, (kptr_t)size, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

int extension_create_file(kptr_t saveto, kptr_t sb, const char *path, size_t path_len, uint32_t subtype) {
    init_function();
    int ret = -1;
    kptr_t kstr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(saveto));
    _assert(KERN_POINTER_VALID(sb));
    _assert(path != NULL);
    _assert(path_len > 0);
    kptr_t function = kernel_offset(extension_create_file);
    _assert(KERN_POINTER_VALID(function));
    kstr = sstrdup(path);
    _assert(KERN_POINTER_VALID(kstr));
    ret = (int)kexec(function, saveto, sb, kstr, (kptr_t)path_len, (kptr_t)subtype, KPTR_NULL, KPTR_NULL);
out:;
    SafeSFreeNULL(kstr);
    return ret;
}

int extension_create_mach(kptr_t saveto, kptr_t sb, const char *name, uint32_t subtype) {
    init_function();
    int ret = -1;
    kptr_t kstr = KPTR_NULL;
    kptr_t function = kernel_offset(extension_create_mach);
    _assert(KERN_POINTER_VALID(function));
    kstr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(saveto));
    _assert(KERN_POINTER_VALID(sb));
    _assert(name != NULL);
    kstr = sstrdup(name);
    _assert(KERN_POINTER_VALID(kstr));
    ret = (int)kexec(function, saveto, sb, kstr, (kptr_t)subtype, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    SafeSFreeNULL(kstr);
    return ret;
}

int extension_add(kptr_t ext, kptr_t sb, const char *desc) {
    init_function();
    int ret = -1;
    kptr_t kstr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(ext));
    _assert(KERN_POINTER_VALID(sb));
    _assert(desc != NULL);
    kptr_t function = kernel_offset(extension_add);
    _assert(KERN_POINTER_VALID(function));
    kstr = sstrdup(desc);
    _assert(KERN_POINTER_VALID(kstr));
    ret = (int)kexec(function, ext, sb, kstr, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    SafeSFreeNULL(kstr);
    return ret;
}

void extension_release(kptr_t ext) {
    init_function();
    _assert(KERN_POINTER_VALID(ext));
    kptr_t function = kernel_offset(extension_release);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, ext, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void extension_destroy(kptr_t ext) {
    init_function();
    _assert(KERN_POINTER_VALID(ext));
    kptr_t function = kernel_offset(extension_destroy);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, ext, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

bool set_file_extension(kptr_t sandbox, const char *exc_key, const char *path) {
    init_function();
    bool ret = false;
    kptr_t ext_kptr = KPTR_NULL;
    kptr_t ext = KPTR_NULL;
    _assert(KERN_POINTER_VALID(sandbox));
    _assert(exc_key != NULL);
    _assert(path != NULL);
    ext_kptr = smalloc(sizeof(kptr_t));
    _assert(KERN_POINTER_VALID(ext_kptr));
    _assert(extension_create_file(ext_kptr, sandbox, path, strlen(path), 0) == 0);
    ext = ReadKernel64(ext_kptr);
    _assert(KERN_POINTER_VALID(ext));
    _assert(extension_add(ext, sandbox, exc_key) == 0);
    ret = true;
out:;
    if (KERN_POINTER_VALID(ext_kptr) && (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_0 || ext == KPTR_NULL)) extension_release(ext_kptr);
    ext_kptr = KPTR_NULL;
    ext = KPTR_NULL;
    return ret;
}

bool set_mach_extension(kptr_t sandbox, const char *exc_key, const char *name) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(sandbox));
    _assert(exc_key != NULL);
    _assert(name != NULL);
    _assert(issue_extension_for_mach_service(sandbox, KPTR_NULL, name, (void *)exc_key) == 0);
    ret = true;
out:;
    return ret;
}

kptr_t proc_find(pid_t pid) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t function = kernel_offset(proc_find);
    _assert(KERN_POINTER_VALID(function));
    ret = kexec(function, (kptr_t)pid, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    if (ret != KPTR_NULL) ret = zm_fix_addr(ret);
out:;
    return ret;
}

void proc_rele(kptr_t proc) {
    init_function();
    _assert(KERN_POINTER_VALID(proc));
    kptr_t function = kernel_offset(proc_rele);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, proc, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void proc_lock(kptr_t proc) {
    init_function();
    _assert(KERN_POINTER_VALID(proc));
    kptr_t function = kernel_offset(proc_lock);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, proc, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void proc_unlock(kptr_t proc) {
    init_function();
    _assert(KERN_POINTER_VALID(proc));
    kptr_t function = kernel_offset(proc_unlock);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, proc, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void proc_ucred_lock(kptr_t proc) {
    init_function();
    _assert(KERN_POINTER_VALID(proc));
    kptr_t function = kernel_offset(proc_ucred_lock);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, proc, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void proc_ucred_unlock(kptr_t proc) {
    init_function();
    _assert(KERN_POINTER_VALID(proc));
    kptr_t function = kernel_offset(proc_ucred_unlock);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, proc, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void vnode_lock(kptr_t vp) {
    init_function();
    _assert(KERN_POINTER_VALID(vp));
    kptr_t function = kernel_offset(vnode_lock);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, vp, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void vnode_unlock(kptr_t vp) {
    init_function();
    _assert(KERN_POINTER_VALID(vp));
    kptr_t function = kernel_offset(vnode_unlock);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, vp, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void mount_lock(kptr_t mp) {
    init_function();
    _assert(KERN_POINTER_VALID(mp));
    kptr_t function = kernel_offset(mount_lock);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, mp, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void mount_unlock(kptr_t mp) {
    init_function();
    _assert(KERN_POINTER_VALID(mp));
    kptr_t function = kernel_offset(mount_unlock);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, mp, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void task_set_platform_binary(kptr_t task, boolean_t is_platform) {
    init_function();
    _assert(KERN_POINTER_VALID(task));
    kptr_t function = kernel_offset(task_set_platform_binary);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, task, (kptr_t)is_platform, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

int chgproccnt(uid_t uid, int diff) {
    init_function();
    int ret = -1;
    kptr_t function = kernel_offset(chgproccnt);
    _assert(KERN_POINTER_VALID(function));
    ret = (int)kexec(function, (kptr_t)uid, (kptr_t)diff, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

void kauth_cred_ref(kptr_t cred) {
    init_function();
    _assert(KERN_POINTER_VALID(cred));
    kptr_t function = kernel_offset(kauth_cred_ref);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, cred, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void kauth_cred_unref(kptr_t cred) {
    init_function();
    _assert(KERN_POINTER_VALID(cred));
    kptr_t function = kernel_offset(kauth_cred_unref);
    _assert(KERN_POINTER_VALID(function));
    kexec(function, cred, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

kptr_t vfs_context_current() {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t function = kernel_offset(vfs_context_current);
    _assert(KERN_POINTER_VALID(function));
    ret = kexec(function, (kptr_t)1, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    if (ret != KPTR_NULL) ret = zm_fix_addr(ret);
out:;
    return ret;
}

int vnode_lookup(const char *path, int flags, kptr_t *vpp, kptr_t ctx) {
    init_function();
    int ret = -1;
    kptr_t kstr = KPTR_NULL;
    size_t vpp_kptr_size = 0;
    kptr_t vpp_kptr = KPTR_NULL;
    _assert(path != NULL);
    _assert(vpp != NULL);
    _assert(KERN_POINTER_VALID(ctx));
    kptr_t function = kernel_offset(vnode_lookup);
    _assert(KERN_POINTER_VALID(function));
    kstr = sstrdup(path);
    _assert(KERN_POINTER_VALID(kstr));
    vpp_kptr_size = sizeof(kptr_t);
    vpp_kptr = smalloc(vpp_kptr_size);
    _assert(KERN_POINTER_VALID(vpp_kptr));
    ret = (int)kexec(function, kstr, (kptr_t)flags, vpp_kptr, ctx, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    _assert(rkbuffer(vpp_kptr, vpp, vpp_kptr_size));
out:;
    SafeSFreeNULL(kstr);
    SafeSFreeNULL(vpp_kptr);
    return ret;
}

int vnode_getfromfd(kptr_t ctx, int fd, kptr_t *vpp) {
    init_function();
    int ret = -1;
    size_t vpp_kptr_size = 0;
    kptr_t vpp_kptr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(ctx));
    _assert(fd > 0);
    _assert(vpp != NULL);
    kptr_t function = kernel_offset(vnode_getfromfd);
    _assert(KERN_POINTER_VALID(function));
    vpp_kptr_size = sizeof(kptr_t);
    vpp_kptr = smalloc(vpp_kptr_size);
    _assert(KERN_POINTER_VALID(vpp_kptr));
    ret = (int)kexec(function, ctx, (kptr_t)fd, vpp_kptr, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    _assert(rkbuffer(vpp_kptr, vpp, vpp_kptr_size));
out:;
    SafeSFreeNULL(vpp_kptr);
    return ret;
}

int vn_getpath(kptr_t vp, char *pathbuf, int *len) {
    init_function();
    int ret = -1;
    size_t pathbuf_kptr_size = 0;
    kptr_t pathbuf_kptr = KPTR_NULL;
    size_t len_kptr_size = 0;
    kptr_t len_kptr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(vp));
    _assert(pathbuf != NULL);
    _assert(len != NULL);
    kptr_t function = kernel_offset(vn_getpath);
    _assert(KERN_POINTER_VALID(function));
    pathbuf_kptr_size = *len;
    pathbuf_kptr = smalloc(pathbuf_kptr_size);
    _assert(KERN_POINTER_VALID(pathbuf_kptr));
    len_kptr_size = sizeof(*len);
    len_kptr = smalloc(len_kptr_size);
    _assert(KERN_POINTER_VALID(len_kptr));
    _assert(wkbuffer(len_kptr, len, len_kptr_size));
    ret = (int)kexec(function, vp, pathbuf_kptr, len_kptr, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    _assert(rkbuffer(pathbuf_kptr, pathbuf, pathbuf_kptr_size));
    _assert(rkbuffer(len_kptr, len, len_kptr_size));
out:;
    SafeSFreeNULL(pathbuf_kptr);
    SafeSFreeNULL(len_kptr);
    return ret;
}

int vnode_put(kptr_t vp) {
    init_function();
    int ret = -1;
    _assert(KERN_POINTER_VALID(vp));
    kptr_t function = kernel_offset(vnode_put);
    _assert(KERN_POINTER_VALID(function));
    ret = (int)kexec(function, vp, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

bool OSDictionary_SetItem(kptr_t OSDictionary, const char *key, kptr_t val) {
    init_function();
    bool ret = false;
    kptr_t kstr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(OSDictionary));
    _assert(key != NULL);
    _assert(KERN_POINTER_VALID(val));
    kptr_t function = OSObjectFunc(OSDictionary, koffset(KVTABLE_OFFSET_OSDICTIONARY_SETOBJECTWITHCHARP));
    _assert(KERN_POINTER_VALID(function));
    kstr = sstrdup(key);
    _assert(KERN_POINTER_VALID(kstr));
    ret = (bool)kexec(function, OSDictionary, kstr, val, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    SafeSFreeNULL(kstr);
    return ret;
}

kptr_t OSDictionary_GetItem(kptr_t OSDictionary, const char *key) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t kstr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(OSDictionary));
    kptr_t function = OSObjectFunc(OSDictionary, koffset(KVTABLE_OFFSET_OSDICTIONARY_GETOBJECTWITHCHARP));
    _assert(KERN_POINTER_VALID(function));
    kstr = sstrdup(key);
    _assert(KERN_POINTER_VALID(kstr));
    ret = kexec(function, OSDictionary, kstr, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    if (ret != KPTR_NULL && (ret>>32) == KPTR_NULL) ret = zm_fix_addr(ret);
out:;
    SafeSFreeNULL(kstr);
    return ret;
}

bool OSDictionary_Merge(kptr_t OSDictionary, kptr_t OSDictionary2) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(OSDictionary));
    _assert(KERN_POINTER_VALID(OSDictionary2));
    kptr_t function = OSObjectFunc(OSDictionary, koffset(KVTABLE_OFFSET_OSDICTIONARY_MERGE));
    _assert(KERN_POINTER_VALID(function));
    ret = (bool)kexec(function, OSDictionary, OSDictionary2, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

uint32_t OSDictionary_ItemCount(kptr_t OSDictionary) {
    init_function();
    uint32_t ret = 0;
    _assert(KERN_POINTER_VALID(OSDictionary));
    ret = ReadKernel32(OSDictionary + 20);
out:;
    return ret;
}

kptr_t OSDictionary_ItemBuffer(kptr_t OSDictionary) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(OSDictionary));
    ret = ReadKernel64(OSDictionary + 32);
out:;
    return ret;
}

kptr_t OSDictionary_ItemKey(kptr_t buffer, uint32_t idx) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(buffer));
    ret = ReadKernel64(buffer + 16 * idx);
out:;
    return ret;
}

kptr_t OSDictionary_ItemValue(kptr_t buffer, uint32_t idx) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(buffer));
    ret = ReadKernel64(buffer + 16 * idx + 8);
out:;
    return ret;
}

bool OSArray_Merge(kptr_t OSArray, kptr_t OSArray2) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(OSArray));
    _assert(KERN_POINTER_VALID(OSArray2));
    kptr_t function = OSObjectFunc(OSArray, koffset(KVTABLE_OFFSET_OSARRAY_MERGE));
    _assert(KERN_POINTER_VALID(function));
    ret = (bool)kexec(function, OSArray, OSArray2, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

kptr_t OSArray_GetObject(kptr_t OSArray, uint32_t idx) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(OSArray));
    kptr_t function = OSObjectFunc(OSArray, koffset(KVTABLE_OFFSET_OSARRAY_GETOBJECT));
    _assert(KERN_POINTER_VALID(function));
    ret = kexec(OSArray, idx, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    if (ret != KPTR_NULL) ret = zm_fix_addr(ret);
    _assert(KERN_POINTER_VALID(ret));
out:;
    return ret;
}

void OSArray_RemoveObject(kptr_t OSArray, uint32_t idx) {
    init_function();
    _assert(KERN_POINTER_VALID(OSArray));
    kptr_t function = OSObjectFunc(OSArray, koffset(KVTABLE_OFFSET_OSARRAY_REMOVEOBJECT));
    _assert(KERN_POINTER_VALID(function));
    kexec(function, OSArray, idx, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

uint32_t OSArray_ItemCount(kptr_t OSArray) {
    init_function();
    uint32_t ret = 0;
    _assert(KERN_POINTER_VALID(OSArray));
    ret = ReadKernel32(OSArray + 0x14);
out:;
    return ret;
}

kptr_t OSArray_ItemBuffer(kptr_t OSArray) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(OSArray));
    ret = ReadKernel64(OSArray + 32);
out:;
    return ret;
}

kptr_t OSObjectFunc(kptr_t OSObject, uint32_t off) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(OSObject));
    kptr_t vtable = ReadKernel64(OSObject);
    if (vtable != KPTR_NULL) vtable = kernel_xpacd(vtable);
    _assert(KERN_POINTER_VALID(vtable));
    ret = ReadKernel64(vtable + (sizeof(kptr_t) * off));
    if (ret != KPTR_NULL) ret = kernel_xpaci(ret);
    _assert(KERN_POINTER_VALID(ret));
out:;
    return ret;
}

void OSObject_Release(kptr_t OSObject) {
    init_function();
    _assert(KERN_POINTER_VALID(OSObject));
    kptr_t function = OSObjectFunc(OSObject, koffset(KVTABLE_OFFSET_OSOBJECT_RELEASE));
    _assert(KERN_POINTER_VALID(function));
    kexec(function, OSObject, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

void OSObject_Retain(kptr_t OSObject) {
    init_function();
    _assert(KERN_POINTER_VALID(OSObject));
    kptr_t function = OSObjectFunc(OSObject, koffset(KVTABLE_OFFSET_OSOBJECT_RETAIN));
    _assert(KERN_POINTER_VALID(function));
    kexec(function, OSObject, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

uint32_t OSObject_GetRetainCount(kptr_t OSObject) {
    init_function();
    uint32_t ret = 0;
    _assert(KERN_POINTER_VALID(OSObject));
    kptr_t function = OSObjectFunc(OSObject, koffset(KVTABLE_OFFSET_OSOBJECT_GETRETAINCOUNT));
    _assert(KERN_POINTER_VALID(function));
    ret = (uint32_t)kexec(function, OSObject, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

uint32_t OSString_GetLength(kptr_t OSString) {
    init_function();
    uint32_t ret = 0;
    _assert(KERN_POINTER_VALID(OSString));
    ret = ReadKernel32(OSString + 0x8) - 1;
out:;
    return ret;
}

kptr_t OSString_CStringPtr(kptr_t OSString) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(OSString));
    ret = ReadKernel64(OSString + 0x10);
out:;
    return ret;
}

char *OSString_CopyString(kptr_t OSString) {
    init_function();
    char *ret = NULL;
    char *str = NULL;
    _assert(KERN_POINTER_VALID(OSString));
    kptr_t CStringPtr = OSString_CStringPtr(OSString);
    _assert(KERN_POINTER_VALID(CStringPtr));
    size_t length = kstrlen(CStringPtr);
    _assert(length > 0);
    str = malloc(length + 1);
    _assert(str != NULL);
    str[length] = 0;
    _assert(rkbuffer(CStringPtr, str, length));
    ret = strdup(str);
    _assert(ret != NULL);
out:;
    SafeFreeNULL(str);
    return ret;
}

kptr_t OSUnserializeXML(const char *buffer) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t kstr = KPTR_NULL;
    _assert(buffer != NULL);
    kptr_t function = kernel_offset(osunserializexml);
    _assert(KERN_POINTER_VALID(function));
    kstr = sstrdup(buffer);
    _assert(KERN_POINTER_VALID(kstr));
    kptr_t error_kptr = KPTR_NULL;
    ret = kexec(function, kstr, error_kptr, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    if (ret != KPTR_NULL) ret = zm_fix_addr(ret);
    _assert(KERN_POINTER_VALID(ret));
out:;
    SafeSFreeNULL(kstr);
    return ret;
}

kptr_t get_exception_osarray(const char **exceptions, bool is_file_extension) {
    init_function();
    kptr_t exception_osarray = KPTR_NULL;
    size_t xmlsize = 0x1000;
    size_t len = 0;
    size_t written = 0;
    char *ents = malloc(xmlsize);
    if (ents == NULL) return KPTR_NULL;
    size_t xmlused = sprintf(ents, "<array>");
    for (const char **exception = exceptions; *exception; exception++) {
        len = strlen(*exception);
        len += strlen("<string></string>");
        while (xmlused + len >= xmlsize) {
            xmlsize += 0x1000;
            ents = reallocf(ents, xmlsize);
            if (!ents) {
                return 0;
            }
        }
        written = sprintf(ents + xmlused, "<string>%s%s</string>", *exception, is_file_extension ? "/" : "");
        if (written < 0) {
            SafeFreeNULL(ents);
            return 0;
        }
        xmlused += written;
    }
    len = strlen("</array>");
    if (xmlused + len >= xmlsize) {
        xmlsize += len;
        ents = reallocf(ents, xmlsize);
        if (!ents) {
            return 0;
        }
    }
    written = sprintf(ents + xmlused, "</array>");
    
    exception_osarray = OSUnserializeXML(ents);
    SafeFreeNULL(ents);
    return exception_osarray;
}

char **copy_amfi_entitlements(kptr_t present) {
    init_function();
    uint32_t itemCount = OSArray_ItemCount(present);
    kptr_t itemBuffer = OSArray_ItemBuffer(present);
    size_t bufferSize = 0x1000;
    size_t bufferUsed = 0;
    size_t arraySize = (itemCount + 1) * sizeof(char *);
    char **entitlements = malloc(arraySize + bufferSize);
    if (entitlements == NULL) return NULL;
    entitlements[itemCount] = NULL;
    
    for (int i = 0; i < itemCount; i++) {
        kptr_t item = ReadKernel64(itemBuffer + (i * sizeof(kptr_t)));
        char *entitlementString = OSString_CopyString(item);
        if (!entitlementString) {
            SafeFreeNULL(entitlements);
            return NULL;
        }
        size_t len = strlen(entitlementString) + 1;
        while (bufferUsed + len > bufferSize) {
            bufferSize += 0x1000;
            entitlements = realloc(entitlements, arraySize + bufferSize);
            if (!entitlements) {
                SafeFreeNULL(entitlementString);
                return NULL;
            }
        }
        entitlements[i] = (char*)entitlements + arraySize + bufferUsed;
        strcpy(entitlements[i], entitlementString);
        bufferUsed += len;
        SafeFreeNULL(entitlementString);
    }
    return entitlements;
}

kptr_t getOSBool(bool value) {
    init_function();
    kptr_t ret = KPTR_NULL;
    if (weird_offsets) {
        if (value) {
            ret = kernel_offset(OSBoolean_True);
        } else {
            ret = kernel_offset(OSBoolean_False);
        }
        goto out;
    }
    kptr_t symbol = kernel_offset(OSBoolean_True);
    _assert(KERN_POINTER_VALID(symbol));
    kptr_t OSBool = ReadKernel64(symbol);
    _assert(KERN_POINTER_VALID(OSBool));
    if (!value) OSBool += sizeof(kptr_t);
    ret = OSBool;
out:;
    return ret;
}

bool entitle_process(kptr_t amfi_entitlements, const char *key, kptr_t val) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(amfi_entitlements));
    _assert(key != NULL);
    _assert(KERN_POINTER_VALID(val));
    _assert((ret = OSDictionary_SetItem(amfi_entitlements, key, val)));
out:;
    return ret;
}

bool set_sandbox_exceptions(kptr_t sandbox) {
    init_function();
    bool ret = false;
    _assert(KERN_POINTER_VALID(sandbox));
    for (const char **exception = file_read_exceptions; *exception; exception++) {
        _assert(set_file_extension(sandbox, FILE_READ_EXC_KEY, *exception));
    }
    for (const char **exception = file_read_write_exceptions; *exception; exception++) {
        _assert(set_file_extension(sandbox, FILE_READ_WRITE_EXC_KEY, *exception));
    }
    for (const char **exception = mach_lookup_exceptions; *exception; exception++) {
        _assert(set_mach_extension(sandbox, MACH_LOOKUP_EXC_KEY, *exception));
    }
    for (const char **exception = mach_register_exceptions; *exception; exception++) {
        _assert(set_mach_extension(sandbox, MACH_REGISTER_EXC_KEY, *exception));
    }
    ret = true;
out:;
    return ret;
}

bool check_for_exception(char **current_exceptions, const char *exception) {
    init_function();
    bool ret = false;
    _assert(current_exceptions != NULL);
    _assert(exception != NULL);
    for (char **entitlement_string = current_exceptions; *entitlement_string && !ret; entitlement_string++) {
        char *ent = strdup(*entitlement_string);
        _assert(ent != NULL);
        size_t lastchar = strlen(ent) - 1;
        if (ent[lastchar] == '/') ent[lastchar] = '\0';
        if (strcmp(ent, exception) == 0) {
            ret = true;
        }
        SafeFreeNULL(ent);
    }
out:;
    return ret;
}

bool set_amfi_exceptions(kptr_t amfi_entitlements, const char *exc_key, const char **exceptions, bool is_file_extension) {
    init_function();
    bool ret = false;
    char **current_exceptions = NULL;
    _assert(KERN_POINTER_VALID(amfi_entitlements));
    _assert(exceptions != NULL);
    kptr_t present_exception_osarray = OSDictionary_GetItem(amfi_entitlements, exc_key);
    if (present_exception_osarray == KPTR_NULL) {
        kptr_t osarray = get_exception_osarray(exceptions, is_file_extension);
        _assert(KERN_POINTER_VALID(osarray));
        ret = OSDictionary_SetItem(amfi_entitlements, exc_key, osarray);
        OSObject_Release(osarray);
        goto out;
    }
    current_exceptions = copy_amfi_entitlements(present_exception_osarray);
    _assert(current_exceptions != NULL);
    for (const char **exception = exceptions; *exception; exception++) {
        if (check_for_exception(current_exceptions, *exception)) {
            ret = true;
            continue;
        }
        const char *array[] = {*exception, NULL};
        kptr_t osarray = get_exception_osarray(array, is_file_extension);
        if (!KERN_POINTER_VALID(osarray)) continue;
        ret = OSArray_Merge(present_exception_osarray, osarray);
        OSObject_Release(osarray);
    }
out:;
    SafeFreeNULL(current_exceptions);
    return ret;
}

bool set_exceptions(kptr_t sandbox, kptr_t amfi_entitlements) {
    init_function();
    bool ret = false;
    if (KERN_POINTER_VALID(sandbox)) {
        _assert(set_sandbox_exceptions(sandbox));
        if (KERN_POINTER_VALID(amfi_entitlements)) {
            _assert(set_amfi_exceptions(amfi_entitlements, FILE_READ_EXC_KEY, file_read_exceptions, true));
            _assert(set_amfi_exceptions(amfi_entitlements, FILE_READ_WRITE_EXC_KEY, file_read_write_exceptions, true));
            _assert(set_amfi_exceptions(amfi_entitlements, MACH_LOOKUP_EXC_KEY, mach_lookup_exceptions, false));
            _assert(set_amfi_exceptions(amfi_entitlements, MACH_REGISTER_EXC_KEY, mach_register_exceptions, false));
        }
    }
    ret = true;
out:;
    return ret;
}

kptr_t get_amfi_entitlements(kptr_t cr_label) {
    init_function();
    kptr_t amfi_entitlements = KPTR_NULL;
    _assert(KERN_POINTER_VALID(cr_label));
    amfi_entitlements = ReadKernel64(cr_label + 0x8);
out:;
    return amfi_entitlements;
}

kptr_t get_sandbox(kptr_t cr_label) {
    init_function();
    kptr_t sandbox = KPTR_NULL;
    _assert(KERN_POINTER_VALID(cr_label));
    sandbox = ReadKernel64(cr_label + 0x10);
out:;
    return sandbox;
}

bool entitle_process_with_pid(pid_t pid, const char *key, kptr_t val) {
    init_function();
    bool ret = false;
    kptr_t proc = KPTR_NULL;
    _assert(pid > 0);
    _assert(key != NULL);
    _assert(KERN_POINTER_VALID(val));
    proc = get_proc_struct_for_pid(pid);
    _assert(KERN_POINTER_VALID(proc));
    kptr_t proc_ucred = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_UCRED));
    _assert(KERN_POINTER_VALID(proc_ucred));
    kptr_t cr_label = ReadKernel64(proc_ucred + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL));
    _assert(KERN_POINTER_VALID(cr_label));
    kptr_t amfi_entitlements = get_amfi_entitlements(cr_label);
    _assert(KERN_POINTER_VALID(amfi_entitlements));
    _assert(entitle_process(amfi_entitlements, key, val));
    ret = true;
out:;
    return ret;
}

bool remove_memory_limit() {
    init_function();
    bool ret = false;
    if (!OPT(SKIP_JETSAM_ENTITLEMENT)) {
        _assert(entitle_process_with_pid(getpid(), "com.apple.private.memorystatus", OSBoolTrue));
    }
    _assert(memorystatus_control(MEMORYSTATUS_CMD_SET_JETSAM_TASK_LIMIT, getpid(), 0, NULL, 0) == 0);
    ret = true;
out:;
    return ret;
}

bool restore_kernel_task_port(task_t *out_kernel_task_port) {
    init_function();
    bool restored_kernel_task_port = false;
    kern_return_t kr = KERN_FAILURE;
    task_t *kernel_task_port = NULL;
    host_t host = HOST_NULL;
    _assert(out_kernel_task_port != NULL);
    kernel_task_port = malloc(sizeof(task_t *));
    _assert(kernel_task_port != NULL);
    bzero(kernel_task_port, sizeof(task_t));
    host = mach_host_self();
    _assert(MACH_PORT_VALID(host));
    kr = task_for_pid(mach_task_self(), 0, kernel_task_port);
    if (kr != KERN_SUCCESS) kr = host_get_special_port(host, HOST_LOCAL_NODE, 4, kernel_task_port);
    _assert(kr == KERN_SUCCESS);
    _assert(MACH_PORT_VALID(*kernel_task_port));
    *out_kernel_task_port = *kernel_task_port;
    restored_kernel_task_port = true;
out:;
    SafeFreeNULL(kernel_task_port);
    if (MACH_PORT_VALID(host)) mach_port_deallocate(mach_task_self(), host); host = HOST_NULL;
    return restored_kernel_task_port;
}

bool restore_kernel_base(uint64_t *out_kernel_base, uint64_t *out_kernel_slide) {
    init_function();
    bool restored_kernel_base = false;
    kern_return_t kr = KERN_FAILURE;
    kptr_t *kernel_task_base = NULL;
    uint64_t *kernel_task_slide = NULL;
    struct task_dyld_info *task_dyld_info = NULL;
    mach_msg_type_number_t *task_dyld_info_count = NULL;
    _assert(out_kernel_base != NULL);
    _assert(out_kernel_slide != NULL);
    kernel_task_base = malloc(sizeof(kptr_t));
    _assert(kernel_task_base != NULL);
    bzero(kernel_task_base, sizeof(kptr_t));
    kernel_task_slide = malloc(sizeof(uint64_t));
    _assert(kernel_task_slide != NULL);
    bzero(kernel_task_slide, sizeof(uint64_t));
    task_dyld_info = malloc(sizeof(struct task_dyld_info));
    _assert(task_dyld_info != NULL);
    bzero(task_dyld_info, sizeof(struct task_dyld_info));
    task_dyld_info_count = malloc(sizeof(mach_msg_type_number_t));
    _assert(task_dyld_info_count != NULL);
    bzero(task_dyld_info_count, sizeof(mach_msg_type_number_t));
    *task_dyld_info_count = TASK_DYLD_INFO_COUNT;
    kr = task_info(tfp0, TASK_DYLD_INFO, (task_info_t)task_dyld_info, task_dyld_info_count);
    _assert(kr == KERN_SUCCESS);
    *kernel_task_slide = task_dyld_info->all_image_info_size;
    *kernel_task_base = *kernel_task_slide + STATIC_KERNEL_BASE_ADDRESS;
    *out_kernel_base = *kernel_task_base;
    *out_kernel_slide = *kernel_task_slide;
    restored_kernel_base = true;
out:;
    SafeFreeNULL(kernel_task_base);
    SafeFreeNULL(kernel_task_slide);
    SafeFreeNULL(task_dyld_info);
    SafeFreeNULL(task_dyld_info_count);
    return restored_kernel_base;
}

bool restore_kernel_offset_cache() {
    init_function();
    bool restored_kernel_offset_cache = false;
    kern_return_t kr = KERN_FAILURE;
    struct task_dyld_info *task_dyld_info = NULL;
    mach_msg_type_number_t *task_dyld_info_count = NULL;
    kptr_t offset_cache_addr = KPTR_NULL;
    kptr_t offset_cache_size_addr = KPTR_NULL;
    size_t *offset_cache_size = NULL;
    struct cache_blob *offset_cache_blob = NULL;
    task_dyld_info = malloc(sizeof(struct task_dyld_info));
    _assert(task_dyld_info != NULL);
    bzero(task_dyld_info, sizeof(struct task_dyld_info));
    task_dyld_info_count = malloc(sizeof(mach_msg_type_number_t));
    _assert(task_dyld_info_count != NULL);
    bzero(task_dyld_info_count, sizeof(mach_msg_type_number_t));
    offset_cache_size = malloc(sizeof(size_t));
    _assert(offset_cache_size != NULL);
    bzero(offset_cache_size, sizeof(size_t));
    *task_dyld_info_count = TASK_DYLD_INFO_COUNT;
    kr = task_info(tfp0, TASK_DYLD_INFO, (task_info_t)task_dyld_info, task_dyld_info_count);
    _assert(kr == KERN_SUCCESS);
    _assert(KERN_POINTER_VALID(task_dyld_info->all_image_info_addr));
    offset_cache_addr = task_dyld_info->all_image_info_addr;
    _assert(offset_cache_addr != kernel_base);
	_assert(ReadKernel32(offset_cache_addr) != MH_MAGIC_64);
    offset_cache_size_addr = offset_cache_addr + offsetof(struct cache_blob, size);
    _assert(rkbuffer(offset_cache_size_addr, offset_cache_size, sizeof(*offset_cache_size)));
    offset_cache_blob = create_cache_blob(*offset_cache_size);
    _assert(offset_cache_blob != NULL);
    _assert(rkbuffer(offset_cache_addr, offset_cache_blob, *offset_cache_size));
    import_cache_blob(offset_cache_blob);
    found_offsets = true;
    restored_kernel_offset_cache = true;
out:;
    SafeFreeNULL(task_dyld_info);
    SafeFreeNULL(task_dyld_info_count);
    SafeFreeNULL(offset_cache_size);
    SafeFreeNULL(offset_cache_blob);
    return restored_kernel_offset_cache;
}

bool restore_file_offset_cache(const char *offset_cache_file_path, kptr_t *out_kernel_base, uint64_t *out_kernel_slide) {
    init_function();
    bool restored_file_offset_cache = false;
    CFStringRef offset_cache_file_name = NULL;
    CFURLRef offset_cache_file_url = NULL;
    CFDataRef offset_cache_file_data = NULL;
    CFPropertyListRef offset_cache_property_list = NULL;
    Boolean status = false;
    kptr_t offset_kernel_base = KPTR_NULL;
    kptr_t offset_kernel_slide = KPTR_NULL;
    _assert(offset_cache_file_path != NULL);
    _assert(out_kernel_base != NULL);
    _assert(out_kernel_slide != NULL);
    offset_cache_file_name = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, offset_cache_file_path, kCFStringEncodingUTF8, kCFAllocatorDefault);
    _assert(offset_cache_file_name != NULL);
    offset_cache_file_url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, offset_cache_file_name, kCFURLPOSIXPathStyle, false);
    _assert(offset_cache_file_url != NULL);
    status = CFURLCreateDataAndPropertiesFromResource(kCFAllocatorDefault, offset_cache_file_url, &offset_cache_file_data, NULL, NULL, NULL);
    _assert(status);
    offset_cache_property_list = CFPropertyListCreateWithData(kCFAllocatorDefault, offset_cache_file_data, kCFPropertyListImmutable, NULL, NULL);
    _assert(offset_cache_property_list);
    _assert(CFGetTypeID(offset_cache_property_list) == CFDictionaryGetTypeID());
#define restore_offset(entry_name, out_offset) do { \
    const void *value = CFDictionaryGetValue(offset_cache_property_list, CFSTR(entry_name)); \
    if (value == NULL) break; \
    const char *string = CFStringGetCStringPtr((CFStringRef)value, kCFStringEncodingUTF8); \
    if (string == NULL) break; \
    uint64_t offset = strtoull(string, NULL, 16); \
    if (!KERN_POINTER_VALID(offset)) break; \
    out_offset = offset; \
} while (false)
#define restore_and_set_offset(entry_name, offset_name) do { \
    kptr_t restored_offset = KPTR_NULL; \
    restore_offset(entry_name, restored_offset); \
    set_offset(offset_name, restored_offset); \
} while (false)
    restore_offset("KernelBase", offset_kernel_base);
    restore_offset("KernelSlide", offset_kernel_slide);
    restore_and_set_offset("TrustChain", "trustcache");
    restore_and_set_offset("OSBooleanTrue", "OSBoolean_True");
    restore_and_set_offset("OSBooleanFalse", "OSBoolean_False");
    restore_and_set_offset("OSUnserializeXML", "osunserializexml");
    restore_and_set_offset("Smalloc", "smalloc");
    restore_and_set_offset("AddRetGadget", "add_x0_x0_0x40_ret");
    restore_and_set_offset("ZoneMapOffset", "zone_map_ref");
    restore_and_set_offset("VfsContextCurrent", "vfs_context_current");
    restore_and_set_offset("VnodeLookup", "vnode_lookup");
    restore_and_set_offset("VnodePut", "vnode_put");
    restore_and_set_offset("KernelTask", "kernel_task");
    restore_and_set_offset("KernProc", "kernproc");
    restore_and_set_offset("Shenanigans", "shenanigans");
    restore_and_set_offset("LckMtxLock", "lck_mtx_lock");
    restore_and_set_offset("LckMtxUnlock", "lck_mtx_unlock");
    restore_and_set_offset("VnodeGetSnapshot", "vnode_get_snapshot");
    restore_and_set_offset("FsLookupSnapshotMetadataByNameAndReturnName", "fs_lookup_snapshot_metadata_by_name_and_return_name");
    restore_and_set_offset("PmapLoadTrustCache", "pmap_load_trust_cache");
    restore_and_set_offset("APFSJhashGetVnode", "apfs_jhash_getvnode");
    restore_and_set_offset("PacizaPointerL2TPDomainModuleStart", "paciza_pointer__l2tp_domain_module_start");
    restore_and_set_offset("PacizaPointerL2TPDomainModuleStop", "paciza_pointer__l2tp_domain_module_stop");
    restore_and_set_offset("L2TPDomainInited", "l2tp_domain_inited");
    restore_and_set_offset("SysctlNetPPPL2TP", "sysctl__net_ppp_l2tp");
    restore_and_set_offset("SysctlUnregisterOid", "sysctl_unregister_oid");
    restore_and_set_offset("MovX0X4BrX5", "mov_x0_x4__br_x5");
    restore_and_set_offset("MovX9X0BrX1", "mov_x9_x0__br_x1");
    restore_and_set_offset("MovX10X3BrX6", "mov_x10_x3__br_x6");
    restore_and_set_offset("KernelForgePaciaGadget", "kernel_forge_pacia_gadget");
    restore_and_set_offset("KernelForgePacdaGadget", "kernel_forge_pacda_gadget");
    restore_and_set_offset("IOUserClientVtable", "IOUserClient__vtable");
    restore_and_set_offset("IORegistryEntryGetRegistryEntryID", "IORegistryEntry__getRegistryEntryID");
    restore_and_set_offset("ProcFind", "proc_find");
    restore_and_set_offset("ProcRele", "proc_rele");
    restore_and_set_offset("ExtensionCreateFile", "extension_create_file");
    restore_and_set_offset("ExtensionAdd", "extension_add");
    restore_and_set_offset("ExtensionRelease", "extension_release");
    restore_and_set_offset("Sfree", "sfree");
    restore_and_set_offset("Sstrdup", "sstrdup");
    restore_and_set_offset("Strlen", "strlen");
#undef restore_offset
#undef restore_and_set_offset
    *out_kernel_base = offset_kernel_base;
    *out_kernel_slide = offset_kernel_slide;
    weird_offsets = true;
    found_offsets = true;
    restored_file_offset_cache = true;
out:;
    CFSafeReleaseNULL(offset_cache_file_url);
    CFSafeReleaseNULL(offset_cache_file_data);
    CFSafeReleaseNULL(offset_cache_property_list);
    return restored_file_offset_cache;
}

bool convert_port_to_task_port(mach_port_t port, kptr_t space, kptr_t task_kaddr) {
    init_function();
    bool ret = false;
    _assert(MACH_PORT_VALID(port));
    _assert(KERN_POINTER_VALID(space));
    _assert(KERN_POINTER_VALID(task_kaddr));
    kptr_t port_kaddr = get_address_of_port(proc_struct_addr(), port);
    _assert(KERN_POINTER_VALID(port_kaddr));
    _assert(WriteKernel32(port_kaddr + koffset(KSTRUCT_OFFSET_IPC_PORT_IO_BITS), IO_BITS_ACTIVE | IKOT_TASK));
    _assert(WriteKernel32(port_kaddr + koffset(KSTRUCT_OFFSET_IPC_PORT_IO_REFERENCES), 0xf00d));
    _assert(WriteKernel32(port_kaddr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_SRIGHTS), 0xf00d));
    _assert(WriteKernel64(port_kaddr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_RECEIVER), space));
    _assert(WriteKernel64(port_kaddr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT),  task_kaddr));
    kptr_t task_port_addr = task_self_addr();
    _assert(KERN_POINTER_VALID(task_port_addr));
    kptr_t task_addr = ReadKernel64(task_port_addr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
    _assert(KERN_POINTER_VALID(task_addr));
    kptr_t itk_space = ReadKernel64(task_addr + koffset(KSTRUCT_OFFSET_TASK_ITK_SPACE));
    _assert(KERN_POINTER_VALID(itk_space));
    kptr_t is_table = ReadKernel64(itk_space + koffset(KSTRUCT_OFFSET_IPC_SPACE_IS_TABLE));
    _assert(KERN_POINTER_VALID(is_table));
    uint32_t bits = ReadKernel32(is_table + (MACH_PORT_INDEX(port) * koffset(KSTRUCT_SIZE_IPC_ENTRY)) + koffset(KSTRUCT_OFFSET_IPC_ENTRY_IE_BITS));
    bits &= (~IE_BITS_RECEIVE);
    bits |= IE_BITS_SEND;
    _assert(WriteKernel32(is_table + (MACH_PORT_INDEX(port) * koffset(KSTRUCT_SIZE_IPC_ENTRY)) + koffset(KSTRUCT_OFFSET_IPC_ENTRY_IE_BITS), bits));
    ret = true;
out:;
    return ret;
}

kptr_t make_fake_task(kptr_t vm_map) {
    init_function();
    kptr_t ret = KPTR_NULL;
    size_t fake_task_size = 0;
    kptr_t fake_task_kaddr = KPTR_NULL;
    void *fake_task = NULL;
    _assert(KERN_POINTER_VALID(vm_map));
    fake_task_size = 0x1000;
    fake_task_kaddr = kmem_alloc(fake_task_size);
    _assert(KERN_POINTER_VALID(fake_task_kaddr));
    fake_task = malloc(fake_task_size);
    _assert(fake_task != NULL);
    memset(fake_task, 0, fake_task_size);
    *(uint32_t*)(fake_task + koffset(KSTRUCT_OFFSET_TASK_REF_COUNT)) = 0xd00d;
    *(uint32_t*)(fake_task + koffset(KSTRUCT_OFFSET_TASK_ACTIVE)) = 1;
    *(uint64_t*)(fake_task + koffset(KSTRUCT_OFFSET_TASK_VM_MAP)) = vm_map;
    *(uint8_t*)(fake_task + koffset(KSTRUCT_OFFSET_TASK_LCK_MTX_TYPE)) = 0x22;
    _assert(wkbuffer(fake_task_kaddr, fake_task, fake_task_size));
    ret = fake_task_kaddr;
out:;
    if (!KERN_POINTER_VALID(ret)) SafeKernelFreeNULL(fake_task_kaddr, fake_task_size);
    SafeFreeNULL(fake_task);
    return ret;
}

bool make_port_fake_task_port(mach_port_t port, kptr_t task_kaddr) {
    init_function();
    bool ret = false;
    _assert(MACH_PORT_VALID(port));
    _assert(KERN_POINTER_VALID(task_kaddr));
    _assert(convert_port_to_task_port(port, _ipc_space_kernel, task_kaddr));
    ret = true;
out:;
    return ret;
}

bool remap_tfp0_set_hspn(int slot, task_t port) {
    init_function();
    bool ret = false;
    host_t host = HOST_NULL;
    kern_return_t kr = KERN_FAILURE;
    _assert(MACH_PORT_VALID(port));
    host = mach_host_self();
    _assert(MACH_PORT_VALID(host));
    size_t const sizeof_task = 0x1000;
    task_t zm_fake_task_port = TASK_NULL;
    task_t km_fake_task_port = TASK_NULL;
    kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &zm_fake_task_port);
    _assert(kr == KERN_SUCCESS);
    kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &km_fake_task_port);
    _assert(kr == KERN_SUCCESS);
    kptr_t zm_fake_task_addr = make_fake_task(_zone_map);
    _assert(KERN_POINTER_VALID(zm_fake_task_addr));
    kptr_t km_fake_task_addr = make_fake_task(_kernel_map);
    _assert(KERN_POINTER_VALID(km_fake_task_addr));
    _assert(make_port_fake_task_port(zm_fake_task_port, zm_fake_task_addr));
    _assert(make_port_fake_task_port(km_fake_task_port, km_fake_task_addr));
    km_fake_task_port = zm_fake_task_port;
    vm_prot_t cur = VM_PROT_NONE, max = VM_PROT_NONE;
    kptr_t remapped_task_addr = KPTR_NULL;
    kr = mach_vm_remap(km_fake_task_port, &remapped_task_addr, sizeof_task, 0, VM_FLAGS_ANYWHERE | VM_FLAGS_RETURN_DATA_ADDR, zm_fake_task_port, _kernel_task, 0, &cur, &max, VM_INHERIT_NONE);
    _assert(kr == KERN_SUCCESS);
    _assert(remapped_task_addr != _kernel_task);
    kr = mach_vm_wire(host, km_fake_task_port, remapped_task_addr, sizeof_task, VM_PROT_READ | VM_PROT_WRITE);
    _assert(kr == KERN_SUCCESS);
    kptr_t port_addr = get_address_of_port(proc_struct_addr(), port);
    _assert(KERN_POINTER_VALID(port_addr));
    _assert(make_port_fake_task_port(port, remapped_task_addr));
    _assert(ReadKernel64(port_addr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT)) == remapped_task_addr);
    kptr_t host_priv_addr = get_address_of_port(proc_struct_addr(), host);
    _assert(KERN_POINTER_VALID(host_priv_addr));
    kptr_t realhost_addr = ReadKernel64(host_priv_addr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
    _assert(KERN_POINTER_VALID(realhost_addr));
    _assert(WriteKernel64(realhost_addr + koffset(KSTRUCT_OFFSET_HOST_SPECIAL) + slot * sizeof(kptr_t), port_addr));
    ret = true;
out:;
    if (MACH_PORT_VALID(host)) mach_port_deallocate(mach_task_self(), host); host = HOST_NULL;
    return ret;
}

kptr_t get_vnode_for_path(const char *path) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t *vpp = NULL;
    _assert(path != NULL);
    kptr_t vfs_context = vfs_context_current();
    _assert(KERN_POINTER_VALID(vfs_context));
    vpp = malloc(sizeof(kptr_t));
    _assert(vpp != NULL);
    bzero(vpp, sizeof(kptr_t));
    _assert(vnode_lookup(path, O_RDONLY, vpp, vfs_context) == 0);
    kptr_t vnode = *vpp;
    _assert(KERN_POINTER_VALID(vnode));
    ret = vnode;
out:;
    SafeFreeNULL(vpp);
    return ret;
}

kptr_t get_vnode_for_fd(int fd) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t *vpp = NULL;
    _assert(fd > 0);
    kptr_t vfs_context = vfs_context_current();
    _assert(KERN_POINTER_VALID(vfs_context));
    vpp = malloc(sizeof(kptr_t));
    _assert(vpp != NULL);
    bzero(vpp, sizeof(kptr_t));
    _assert(vnode_getfromfd(vfs_context, fd, vpp) == 0);
    kptr_t vnode = *vpp;
    _assert(KERN_POINTER_VALID(vnode));
    ret = vnode;
out:;
    SafeFreeNULL(vpp);
    return ret;
}

char *get_path_for_vnode(kptr_t vnode) {
    init_function();
    char *ret = NULL;
    int *len = NULL;
    char *pathbuf = NULL;
    _assert(KERN_POINTER_VALID(vnode));
    len = malloc(sizeof(int));
    _assert(len != NULL);
    *len = MAXPATHLEN;
    pathbuf = malloc(*len);
    _assert(pathbuf != NULL);
    _assert(vn_getpath(vnode, pathbuf, len) == 0);
    _assert(strlen(pathbuf) + 1 == *len);
    ret = strdup(pathbuf);
out:;
    SafeFreeNULL(pathbuf);
    SafeFreeNULL(len);
    return ret;
}

kptr_t get_vnode_for_snapshot(int fd, char *name) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t snap_vnode, rvpp_ptr, sdvpp_ptr, ndp_buf, sdvpp, snap_meta_ptr, old_name_ptr, ndp_old_name;
    snap_vnode = rvpp_ptr = sdvpp_ptr = ndp_buf = sdvpp = snap_meta_ptr = old_name_ptr = ndp_old_name = KPTR_NULL;
    size_t rvpp_ptr_size, sdvpp_ptr_size, ndp_buf_size, snap_meta_ptr_size, old_name_ptr_size;
    ndp_buf_size = 816;
    rvpp_ptr_size = sdvpp_ptr_size = snap_meta_ptr_size = old_name_ptr_size = sizeof(kptr_t);
    rvpp_ptr = kmem_alloc(rvpp_ptr_size);
    _assert(KERN_POINTER_VALID(rvpp_ptr));
    sdvpp_ptr = kmem_alloc(sdvpp_ptr_size);
    _assert(KERN_POINTER_VALID(sdvpp_ptr));
    ndp_buf = kmem_alloc(ndp_buf_size);
    _assert(KERN_POINTER_VALID(ndp_buf));
    kptr_t vfs_context = vfs_context_current();
    _assert(KERN_POINTER_VALID(vfs_context));
    _assert(kexec(kernel_offset(vnode_get_snapshot), fd, rvpp_ptr, sdvpp_ptr, (kptr_t)name, ndp_buf, 2, vfs_context) == 0);
    sdvpp = ReadKernel64(sdvpp_ptr);
    _assert(KERN_POINTER_VALID(sdvpp_ptr));
    kptr_t sdvpp_v_mount = ReadKernel64(sdvpp + koffset(KSTRUCT_OFFSET_VNODE_V_MOUNT));
    _assert(KERN_POINTER_VALID(sdvpp_v_mount));
    kptr_t sdvpp_v_mount_mnt_data = ReadKernel64(sdvpp_v_mount + koffset(KSTRUCT_OFFSET_MOUNT_MNT_DATA));
    _assert(KERN_POINTER_VALID(sdvpp_v_mount_mnt_data));
    snap_meta_ptr = kmem_alloc(snap_meta_ptr_size);
    _assert(KERN_POINTER_VALID(snap_meta_ptr));
    old_name_ptr = kmem_alloc(old_name_ptr_size);
    _assert(KERN_POINTER_VALID(old_name_ptr));
    ndp_old_name = ReadKernel64(ndp_buf + 336 + 40);
    _assert(KERN_POINTER_VALID(ndp_old_name));
    kptr_t ndp_old_name_len = ReadKernel32(ndp_buf + 336 + 48);
    _assert(kexec(kernel_offset(fs_lookup_snapshot_metadata_by_name_and_return_name), sdvpp_v_mount_mnt_data, ndp_old_name, ndp_old_name_len, snap_meta_ptr, old_name_ptr, 0, 0) == 0);
    kptr_t snap_meta = ReadKernel64(snap_meta_ptr);
    _assert(KERN_POINTER_VALID(snap_meta));
    snap_vnode = kexec(kernel_offset(apfs_jhash_getvnode), sdvpp_v_mount_mnt_data, ReadKernel32(sdvpp_v_mount_mnt_data + 0x1b8), ReadKernel64(snap_meta + sizeof(kptr_t)), 1, 0, 0, 0);
    if (snap_vnode == KPTR_NULL) snap_vnode = kexec(kernel_offset(apfs_jhash_getvnode), sdvpp_v_mount_mnt_data, ReadKernel32(sdvpp_v_mount_mnt_data + 0x1d0), ReadKernel64(snap_meta + sizeof(kptr_t)), 1, 0, 0, 0);
    if (snap_vnode != KPTR_NULL) snap_vnode = zm_fix_addr(snap_vnode);
    _assert(KERN_POINTER_VALID(snap_vnode));
    ret = snap_vnode;
out:
    if (KERN_POINTER_VALID(sdvpp)) vnode_put(sdvpp); sdvpp = KPTR_NULL;
    SafeKernelFreeNULL(rvpp_ptr, rvpp_ptr_size);
    SafeKernelFreeNULL(sdvpp_ptr, sdvpp_ptr_size);
    SafeKernelFreeNULL(ndp_buf, ndp_buf_size);
    SafeKernelFreeNULL(snap_meta_ptr, snap_meta_ptr_size);
    SafeKernelFreeNULL(old_name_ptr, old_name_ptr_size);
    return ret;
}

bool set_kernel_task_info() {
    init_function();
    bool ret = false;
    kern_return_t kr = KERN_FAILURE;
    struct task_dyld_info *task_dyld_info = NULL;
    mach_msg_type_number_t *task_dyld_info_count = NULL;
    struct cache_blob *cache = NULL;
    size_t cache_size = 0;
    kptr_t kernel_cache_blob = KPTR_NULL;
    task_dyld_info = malloc(sizeof(struct task_dyld_info));
    _assert(task_dyld_info != NULL);
    bzero(task_dyld_info, sizeof(struct task_dyld_info));
    task_dyld_info_count = malloc(sizeof(mach_msg_type_number_t));
    _assert(task_dyld_info_count != NULL);
    bzero(task_dyld_info_count, sizeof(mach_msg_type_number_t));
    *task_dyld_info_count = TASK_DYLD_INFO_COUNT;
	kptr_t task_port_addr = get_address_of_port(proc_struct_addr(), tfp0);
	_assert(KERN_POINTER_VALID(task_port_addr));
    kptr_t kernel_task_addr = ReadKernel64(task_port_addr + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
    _assert(KERN_POINTER_VALID(kernel_task_addr));
	kptr_t real_kernel_task_addr = _kernel_task;
	_assert(KERN_POINTER_VALID(real_kernel_task_addr));
    kr = task_info(tfp0, TASK_DYLD_INFO, (task_info_t)task_dyld_info, task_dyld_info_count);
    _assert(kr == KERN_SUCCESS);
    if (KERN_POINTER_VALID(task_dyld_info->all_image_info_addr) && task_dyld_info->all_image_info_addr != kernel_base && task_dyld_info->all_image_info_addr > kernel_base) {
        size_t blob_size = ReadKernel32(task_dyld_info->all_image_info_addr + offsetof(struct cache_blob, size));
        _assert(blob_size > 0);
        struct cache_blob *blob = create_cache_blob(blob_size);
        _assert(blob != NULL);
        merge_cache_blob(blob); // Adds any entries that are in kernel but we don't have
        SafeFreeNULL(blob);
        SafeKernelFree(task_dyld_info->all_image_info_addr, blob_size); // Free old offset cache - didn't bother comparing because it's faster to just replace it if it's the same
    }
	set_kernel_offset(has_corpse_footprint, HAS_CORPSE_FOOTPRINT);
    set_kernel_offset(task_offset_all_image_info_addr, koffset(KSTRUCT_OFFSET_TASK_ALL_IMAGE_INFO_ADDR));
	set_kernel_offset(ipc_port_ip_kobject, koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
	set_kernel_offset(ipc_space_is_task, koffset(KSTRUCT_OFFSET_IPC_SPACE_IS_TASK));
	set_kernel_offset(task_bsd_info, koffset(KSTRUCT_OFFSET_TASK_BSD_INFO));
	set_kernel_offset(proc_p_pid, koffset(KSTRUCT_OFFSET_PROC_PID));
    set_kernel_offset(proc_p_ucred, koffset(KSTRUCT_OFFSET_PROC_UCRED));
    set_kernel_offset(proc_p_svuid, koffset(KSTRUCT_OFFSET_PROC_SVUID));
    set_kernel_offset(proc_p_svgid, koffset(KSTRUCT_OFFSET_PROC_SVGID));
    set_kernel_offset(proc_p_csflags, koffset(KSTRUCT_OFFSET_PROC_P_CSFLAGS));
    set_kernel_offset(proc_p_textvp, koffset(KSTRUCT_OFFSET_PROC_TEXTVP));
    set_kernel_offset(proc_p_flag, koffset(KSTRUCT_OFFSET_PROC_P_FLAG));
    set_kernel_offset(proc_p_fd, koffset(KSTRUCT_OFFSET_PROC_P_FD));
    set_kernel_offset(filedesc_fd_ofiles, koffset(KSTRUCT_OFFSET_FILEDESC_FD_OFILES));
    set_kernel_offset(fileproc_f_fglob, koffset(KSTRUCT_OFFSET_FILEPROC_F_FGLOB));
    set_kernel_offset(fileglob_fg_data, koffset(KSTRUCT_OFFSET_FILEGLOB_FG_DATA));
    set_kernel_offset(proc_task, koffset(KSTRUCT_OFFSET_PROC_TASK));
    set_kernel_offset(ucred_cr_svuid, koffset(KSTRUCT_OFFSET_UCRED_CR_SVUID));
    set_kernel_offset(ucred_cr_uid, koffset(KSTRUCT_OFFSET_UCRED_CR_UID));
    set_kernel_offset(ucred_cr_svgid, koffset(KSTRUCT_OFFSET_UCRED_CR_SVGID));
    set_kernel_offset(ucred_cr_groups, koffset(KSTRUCT_OFFSET_UCRED_CR_GROUPS));
    set_kernel_offset(ucred_cr_label, koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL));
    set_kernel_offset(label_sandbox, sizeof(kptr_t) * 2);
    set_kernel_offset(task_t_flags, koffset(KSTRUCT_OFFSET_TASK_TFLAGS));
    set_kernel_offset(task_vm_map, koffset(KSTRUCT_OFFSET_TASK_VM_MAP));
    set_kernel_offset(vm_map_flags, koffset(KSTRUCT_OFFSET_VM_MAP_FLAGS));
    set_kernel_offset(vnode_v_ubcinfo, koffset(KSTRUCT_OFFSET_VNODE_V_UBCINFO));
    set_kernel_offset(ubc_info_csblobs, koffset(KSTRUCT_OFFSET_UBC_INFO_CSBLOBS));
    set_kernel_offset(ubc_info_cs_add_gen, offsetof(struct ubc_info, cs_add_gen));
    set_kernel_offset(vnode_v_iocount, offsetof(struct vnode_struct, v_iocount));
	set_kernel_offset(task_itk_space, koffset(KSTRUCT_OFFSET_TASK_ITK_SPACE));
	set_kernel_offset(ipc_space_is_table, koffset(KSTRUCT_OFFSET_IPC_SPACE_IS_TABLE));
	set_kernel_offset(size_ipc_entry, koffset(KSTRUCT_SIZE_IPC_ENTRY));
	set_kernel_offset(ipc_port_ip_kobject, koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
	set_kernel_offset(host_special, koffset(KSTRUCT_OFFSET_HOST_SPECIAL));
	set_kernel_offset(ipc_port_io_references, koffset(KSTRUCT_OFFSET_IPC_PORT_IO_REFERENCES));
	set_kernel_offset(ipc_port_ip_strights, koffset(KSTRUCT_OFFSET_IPC_PORT_IP_SRIGHTS));
    cache_size = export_cache_blob(&cache);
    kernel_cache_blob = kmem_alloc_wired(cache_size);
    _assert(KERN_POINTER_VALID(kernel_cache_blob));
    blob_rebase(cache, (kptr_t)cache, kernel_cache_blob);
    _assert(wkbuffer(kernel_cache_blob, cache, cache_size));
    _assert(WriteKernel64(kernel_task_addr + koffset(KSTRUCT_OFFSET_TASK_ALL_IMAGE_INFO_ADDR), kernel_cache_blob));
    _assert(WriteKernel64(kernel_task_addr + koffset(KSTRUCT_OFFSET_TASK_ALL_IMAGE_INFO_SIZE), kernel_slide));
	if (kernel_task_addr != real_kernel_task_addr) {
		_assert(WriteKernel64(real_kernel_task_addr + koffset(KSTRUCT_OFFSET_TASK_ALL_IMAGE_INFO_ADDR), kernel_cache_blob));
		_assert(WriteKernel64(real_kernel_task_addr + koffset(KSTRUCT_OFFSET_TASK_ALL_IMAGE_INFO_SIZE), kernel_slide));
	}
    bzero(task_dyld_info, sizeof(struct task_dyld_info));
    kr = task_info(tfp0, TASK_DYLD_INFO, (task_info_t)task_dyld_info, task_dyld_info_count);
    _assert(kr == KERN_SUCCESS);
    _assert(task_dyld_info->all_image_info_addr == kernel_cache_blob);
    _assert(task_dyld_info->all_image_info_size == kernel_slide);
    ret = true;
out:;
    if (!ret) SafeKernelFreeNULL(kernel_cache_blob, cache_size);
    SafeFreeNULL(task_dyld_info);
    SafeFreeNULL(task_dyld_info_count);
    SafeFreeNULL(cache);
    return ret;
}

int issue_extension_for_mach_service(kptr_t sb, kptr_t ctx, const char *entry_name, void *desc) {
    init_function();
    int ret = -1;
    kptr_t entry_name_kstr = KPTR_NULL;
    kptr_t desc_kstr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(sb));
    _assert(entry_name != NULL);
    _assert(desc != NULL);
    kptr_t function = kernel_offset(issue_extension_for_mach_service);
    _assert(KERN_POINTER_VALID(function));
    entry_name_kstr = sstrdup(entry_name);
    _assert(KERN_POINTER_VALID(entry_name_kstr));
    desc_kstr = sstrdup(desc);
    _assert(KERN_POINTER_VALID(desc_kstr));
    ret = (int)kexec(function, sb, ctx, entry_name_kstr, desc_kstr, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    SafeSFreeNULL(entry_name_kstr);
    SafeSFreeNULL(desc_kstr);
    return ret;
}

bool unrestrict_process(pid_t pid) {
    init_function();
    bool ret = false;
    char path[PROC_PIDPATHINFO_MAXSIZE];
    struct stat *stat = NULL;
    struct blob {
        uint32_t type;
        uint32_t len;
        char data[0];
    } *blob = NULL;
    _assert(pid > 0);
    kptr_t proc = get_proc_struct_for_pid(pid);
    _assert(KERN_POINTER_VALID(proc));
    kptr_t proc_ucred = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_UCRED));
    _assert(KERN_POINTER_VALID(proc_ucred));
    kptr_t cr_label = ReadKernel64(proc_ucred + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL));
    _assert(KERN_POINTER_VALID(cr_label));
    kptr_t amfi_entitlements = get_amfi_entitlements(cr_label);
    _assert(amfi_entitlements != -1);
    kptr_t sandbox = get_sandbox(cr_label);
    _assert(sandbox != -1);
    kptr_t textvp = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_TEXTVP));
    _assert(KERN_POINTER_VALID(textvp));
    kptr_t ubcinfo = ReadKernel64(textvp + koffset(KSTRUCT_OFFSET_VNODE_V_UBCINFO));
    _assert(KERN_POINTER_VALID(ubcinfo));
    kptr_t csblobs = ReadKernel64(ubcinfo + koffset(KSTRUCT_OFFSET_UBC_INFO_CSBLOBS));
    _assert(KERN_POINTER_VALID(csblobs));
    kptr_t csb_entitlements_blob = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_entitlements_blob));
    _assert(KERN_POINTER_VALID(csb_entitlements_blob));
    uint32_t length = ReadKernel32(csb_entitlements_blob + offsetof(CS_GenericBlob, length));
    _assert(length != 0);
    blob = calloc(1, htonl(length));
    _assert(blob != NULL);
    _assert(csops(pid, CS_OPS_ENTITLEMENTS_BLOB, blob, length) == 0);
    bzero(path, sizeof(path));
    _assert(proc_pidpath(pid, path, sizeof(path)) != -1);
    stat = calloc(1, sizeof(struct stat));
    _assert(stat != NULL);
    _assert(lstat(path, stat) != -1);
    if (stat->st_mode & S_ISUID) {
        _assert(WriteKernel32(proc + koffset(KSTRUCT_OFFSET_PROC_SVUID), stat->st_uid));
        _assert(WriteKernel32(proc_ucred + koffset(KSTRUCT_OFFSET_UCRED_CR_SVUID), stat->st_uid));
        _assert(WriteKernel32(proc_ucred + koffset(KSTRUCT_OFFSET_UCRED_CR_UID), stat->st_uid));
    }
    if (stat->st_mode & S_ISGID) {
        _assert(WriteKernel32(proc + koffset(KSTRUCT_OFFSET_PROC_SVGID), stat->st_gid));
        _assert(WriteKernel32(proc_ucred + koffset(KSTRUCT_OFFSET_UCRED_CR_SVGID), stat->st_gid));
        _assert(WriteKernel32(proc_ucred + koffset(KSTRUCT_OFFSET_UCRED_CR_GROUPS), stat->st_gid));
    }
    if (KERN_POINTER_VALID(amfi_entitlements) && !OPT(SKIP_ENTITLEMENTS)) {
        _assert(entitle_process(amfi_entitlements, "com.apple.private.skip-library-validation", OSBoolTrue));
        if (OPT(GET_TASK_ALLOW)) {
            _assert(entitle_process(amfi_entitlements, "get-task-allow", OSBoolTrue));
        }
    }
    if (OPT(GET_TASK_ALLOW)) {
        _assert(set_csflags(proc, CS_GET_TASK_ALLOW, true));
    }
    if (OPT(SET_PLATFORM_BINARY) || strstr(blob->data, "platform-application") != NULL) {
        _assert(set_platform_binary(proc, true));
        _assert(set_cs_platform_binary(proc, true));
        _assert(set_csb_platform_binary(proc, true));
    }
    if (OPT(CS_DEBUGGED)) {
        _assert(set_csflags(proc, CS_DEBUGGED, true));
        _assert(set_csflags(proc, CS_HARD, false));
        _assert(set_csflags(proc, CS_KILL, false));
    }
    if (OPT(SET_CHILDDRAINED)) {
        _assert(set_child_drained(proc, false));
    }
    if (OPT(SET_HAS_CORPSE_FOOTPRINT)) {
        _assert(set_has_corpse_footprint(proc, false));
    }
    do {
        if (!KERN_POINTER_VALID(sandbox)) break;
        if (OPT(SKIP_SANDBOX)) break;
        if (strcmp(path, "/usr/libexec/securityd") == 0) break;
        kptr_t container_profile = kernel_offset(container_profile);
        if (!KERN_POINTER_VALID(container_profile)) goto exceptions;
        kptr_t sandbox_profile = ReadKernel64(sandbox);
        if (!KERN_POINTER_VALID(sandbox_profile)) break;
        _assert(WriteKernel64(sandbox_profile, container_profile));
        if (false) {
        exceptions:;
            _assert(set_exceptions(sandbox, amfi_entitlements));
        }
        
    } while(false);
    ret = true;
out:;
    SafeFreeNULL(stat);
    SafeFreeNULL(blob);
    return ret;
}

bool unrestrict_process_with_task_port(task_t task_port) {
    init_function();
    bool ret = false;
    pid_t pid = 0;
    _assert(pid_for_task(task_port, &pid) == KERN_SUCCESS);
    _assert(unrestrict_process(pid));
    ret = true;
out:;
    return ret;
}

bool revalidate_process(pid_t pid) {
    init_function();
    bool ret = false;
    kptr_t proc = KPTR_NULL;
    uint32_t csflags = 0;
    _assert(pid > 0);
    _assert(csops(pid, CS_OPS_STATUS, &csflags, sizeof(csflags)) != -1);
    if (csflags & CS_VALID) {
        ret = true;
        goto out;
    }
    proc = get_proc_struct_for_pid(pid);
    _assert(KERN_POINTER_VALID(proc));
    _assert(set_csflags(proc, CS_VALID, true));
    ret = true;
out:;
    return ret;
}

bool revalidate_process_with_task_port(task_t task_port) {
    init_function();
    bool ret = false;
    pid_t pid = 0;
    _assert(pid_for_task(task_port, &pid) == KERN_SUCCESS);
    _assert(revalidate_process(pid));
    ret = true;
out:;
    return ret;
}

bool enable_mapping_for_library(pid_t pid, int fd) {
    init_function();
    bool ret = false;
    kptr_t vnode = KPTR_NULL;
    kptr_t proc = KPTR_NULL;
    _assert(pid > 0);
    _assert(fd > 0);
    proc = get_proc_struct_for_pid(pid);
    _assert(KERN_POINTER_VALID(proc));
    vnode = find_vnode_with_fd(proc, fd);
    _assert(KERN_POINTER_VALID(vnode));
    kptr_t v_flags_addr = vnode + koffset(KSTRUCT_OFFSET_VNODE_V_FLAG);
    uint32_t v_flags = ReadKernel32(v_flags_addr);
    v_flags |= VSHARED_DYLD;
    _assert(WriteKernel32(v_flags_addr, v_flags));
    ret = true;
out:;
    return ret;
}

kptr_t find_vnode_with_fd(kptr_t proc, int fd) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(fd > 0);
    _assert(KERN_POINTER_VALID(proc));
    kptr_t fdp = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_P_FD));
    _assert(KERN_POINTER_VALID(fdp));
    kptr_t ofp = ReadKernel64(fdp + koffset(KSTRUCT_OFFSET_FILEDESC_FD_OFILES));
    _assert(KERN_POINTER_VALID(ofp));
    kptr_t fpp = ReadKernel64(ofp + (fd * sizeof(kptr_t)));
    _assert(KERN_POINTER_VALID(fpp));
    kptr_t fgp = ReadKernel64(fpp + koffset(KSTRUCT_OFFSET_FILEPROC_F_FGLOB));
    _assert(KERN_POINTER_VALID(fgp));
    kptr_t vnode = ReadKernel64(fgp + koffset(KSTRUCT_OFFSET_FILEGLOB_FG_DATA));
    _assert(KERN_POINTER_VALID(vnode));
    ret = vnode;
out:;
    return ret;
}

kptr_t swap_sandbox_for_proc(kptr_t proc, kptr_t sandbox) {
    init_function();
    kptr_t ret = KPTR_NULL;
    _assert(KERN_POINTER_VALID(proc));
    kptr_t ucred = ReadKernel64(proc + koffset(KSTRUCT_OFFSET_PROC_UCRED));
    _assert(KERN_POINTER_VALID(ucred));
    kptr_t cr_label = ReadKernel64(ucred + koffset(KSTRUCT_OFFSET_UCRED_CR_LABEL));
    _assert(KERN_POINTER_VALID(cr_label));
    kptr_t sandbox_addr = cr_label + sizeof(kptr_t) + sizeof(kptr_t);
    kptr_t current_sandbox = ReadKernel64(sandbox_addr);
    _assert(WriteKernel64(sandbox_addr, sandbox));
    ret = current_sandbox;
out:;
    return ret;
}

int cs_validate_csblob(kptr_t address, size_t length, kptr_t *rcd, kptr_t *rentitlements) {
    init_function();
    int ret = 0;
    kptr_t rcd_kptr = KPTR_NULL;
    kptr_t ent_kptr = KPTR_NULL;
    _assert(address != KPTR_NULL);
    _assert(length > 0);
    rcd_kptr = kmem_alloc(sizeof(kptr_t));
    _assert(KERN_POINTER_VALID(rcd_kptr));
    ent_kptr = kmem_alloc(sizeof(kptr_t));
    _assert(KERN_POINTER_VALID(ent_kptr));
    _assert(kexec(kernel_offset(cs_validate_csblob), address, (kptr_t)length, rcd_kptr, ent_kptr, KPTR_NULL, KPTR_NULL, KPTR_NULL) == 0);
    if (rcd != NULL) _assert(rkbuffer(rcd_kptr, rcd, sizeof(kptr_t)));
    if (rentitlements != NULL) _assert(rkbuffer(ent_kptr, rentitlements, sizeof(kptr_t)));
    ret = 0;
out:;
    SafeKernelFreeNULL(rcd_kptr, sizeof(kptr_t));
    SafeKernelFreeNULL(ent_kptr, sizeof(kptr_t));
    return ret;
}

void kfree(kptr_t address, vm_size_t size) {
    init_function();
    kptr_t kfree_kptr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(address));
    _assert(size > 0);
    kfree_kptr = kernel_offset(kfree);
    _assert(KERN_POINTER_VALID(kfree_kptr));
    kexec(kfree_kptr, address, (kptr_t)size, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

kptr_t cs_find_md(uint8_t type) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t cs_hash = KPTR_NULL;
    kptr_t cs_find_md_kptr = KPTR_NULL;
    cs_find_md_kptr = kernel_offset(cs_find_md);
    _assert(KERN_POINTER_VALID(cs_find_md_kptr));
    cs_hash = ReadKernel64(cs_find_md_kptr + ((type - 1) * 8));
    _assert(KERN_POINTER_VALID(cs_hash));
    ret = cs_hash;
out:;
    return ret;
}

uint32_t get_mach_offset(const char *path) {
    init_function();
    uint32_t ret = -1;
    FILE *file = NULL;
    uint32_t offset = 0;
    uint32_t *magic = NULL;
    struct fat_header *fat_header = NULL;
    struct fat_arch *fat_arch = NULL;
    file = fopen(path, "rb");
    _assert(file != NULL);
    magic = calloc(1, sizeof(uint32_t));
    _assert(magic != NULL);
    fseek(file, offset, SEEK_SET);
    fread(magic, sizeof(uint32_t), 1, file);
    _assert(*magic == MH_MAGIC_64 || *magic == FAT_CIGAM);
    if (*magic == FAT_CIGAM) {
        uint32_t arch_off = sizeof(struct fat_header);
        fat_header = calloc(1, sizeof(struct fat_header));
        _assert(fat_header != NULL);
        fseek(file, 0, SEEK_SET);
        fread(fat_header, sizeof(struct fat_header), 1, file);
        int nfat_arch = ntohl(fat_header->nfat_arch);
        while (nfat_arch-- > 0) {
            struct fat_arch *fat_arch = calloc(1, sizeof(struct fat_arch));
            if (fat_arch == NULL) {
                continue;
            }
            fseek(file, arch_off, SEEK_SET);
            fread(fat_arch, sizeof(struct fat_arch), 1, file);
            uint32_t fat_arch_offset = ntohl(fat_arch->offset);
            cpu_type_t fat_arch_cputype = ntohl(fat_arch->cputype);
            SafeFreeNULL(fat_arch);
            if (fat_arch_cputype == CPU_TYPE_ARM64) {
                offset = fat_arch_offset;
                break;
            }
            arch_off += sizeof(struct fat_arch);
        }
        _assert(offset != 0);
    }
    ret = offset;
out:;
    if (file != NULL) fclose(file); file = NULL;
    SafeFreeNULL(magic);
    SafeFreeNULL(fat_header);
    SafeFreeNULL(fat_arch);
    return ret;
}

void csblob_entitlements_dictionary_set(kptr_t csblob, kptr_t entitlements) {
    init_function();
    kptr_t csblob_entitlements_dictionary_set_kptr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(csblob));
    _assert(KERN_POINTER_VALID(entitlements));
    csblob_entitlements_dictionary_set_kptr = kernel_offset(csblob_entitlements_dictionary_set);
    _assert(KERN_POINTER_VALID(csblob_entitlements_dictionary_set_kptr));
    kexec(csblob_entitlements_dictionary_set_kptr, csblob, entitlements, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
}

kptr_t kalloc_canblock(vm_size_t *size, boolean_t canblock, kptr_t site) {
    init_function();
    kptr_t ret = KPTR_NULL;
    kptr_t kalloc_canblock_kptr = KPTR_NULL;
    kptr_t size_kptr = KPTR_NULL;
    _assert(size != NULL);
    _assert(KERN_POINTER_VALID(site));
    kalloc_canblock_kptr = kernel_offset(kalloc_canblock);
    _assert(KERN_POINTER_VALID(kalloc_canblock_kptr));
    size_kptr = kmem_alloc(sizeof(vm_size_t));
    _assert(KERN_POINTER_VALID(size_kptr));
    _assert(wkbuffer(size_kptr, size, sizeof(vm_size_t)));
    ret = kexec(kalloc_canblock_kptr, size_kptr, (kptr_t)canblock, size_kptr, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
    if (ret != KPTR_NULL) ret = zm_fix_addr(ret);
    _assert(rkbuffer(size_kptr, (void *)size, sizeof(vm_size_t)));
out:;
    SafeKernelFreeNULL(size_kptr, sizeof(vm_size_t));
    return ret;
}

kern_return_t kernel_memory_allocate(kptr_t map, vm_offset_t *addrp, vm_size_t size, vm_offset_t mask, int flags, uint16_t tag) {
    init_function();
    kern_return_t ret = KERN_FAILURE;
    kptr_t kernel_memory_allocate_kptr = KPTR_NULL;
    kptr_t addrp_kptr = KPTR_NULL;
    _assert(addrp != NULL);
    kernel_memory_allocate_kptr = kernel_offset(kernel_memory_allocate);
    _assert(KERN_POINTER_VALID(kernel_memory_allocate_kptr));
    addrp_kptr = kmem_alloc(sizeof(vm_offset_t));
    _assert(KERN_POINTER_VALID(addrp_kptr));
    _assert(wkbuffer(addrp_kptr, (void *)addrp, sizeof(vm_offset_t)));
    ret = (kern_return_t)kexec(kernel_memory_allocate_kptr, map, addrp_kptr, (kptr_t)size, (kptr_t)mask, (kptr_t)flags, (kptr_t)tag, KPTR_NULL);
    _assert(rkbuffer(addrp_kptr, (void *)addrp, sizeof(vm_offset_t)));
out:;
    SafeKernelFreeNULL(addrp_kptr, sizeof(vm_offset_t));
    return ret;
}

int vnode_getattr(kptr_t vp, kptr_t vap, kptr_t ctx) {
    init_function();
    int ret = 0;
    kptr_t vnode_getattr_kptr = KPTR_NULL;
    _assert(KERN_POINTER_VALID(vp));
    _assert(KERN_POINTER_VALID(vap));
    _assert(KERN_POINTER_VALID(ctx));
    vnode_getattr_kptr = kernel_offset(vnode_getattr);
    _assert(KERN_POINTER_VALID(vnode_getattr_kptr));
    ret = (int)kexec(vnode_getattr_kptr, vp, vap, ctx, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
out:;
    return ret;
}

bool dump_offset_cache(const char *file) {
    init_function();
    bool ret = false;
    FILE *cache = NULL;
    struct cache_blob *cache_blob = NULL;
    size_t cache_size = 0;
    _assert(file != NULL);
    cache = fopen(file, "wb");
    _assert(cache != NULL);
    cache_size = export_cache_blob(&cache_blob);
    _assert(cache_size > 0);
    fwrite(cache_blob, cache_size, 1, cache);
    ret = true;
out:;
    if (cache != NULL) fclose(cache); cache = NULL;
    SafeFreeNULL(cache_blob);
    return ret;
}

bool restore_offset_cache(const char *file) {
    init_function();
    bool ret = false;
    FILE *cache = NULL;
    struct stat *stat = NULL;
    size_t offset_cache_size = 0;
    struct cache_blob *offset_cache_blob = NULL;
    kptr_t kernel_version_kptr = KPTR_NULL;
    if (access(file, F_OK | R_OK) != 0) goto out;
    cache = fopen(file, "rb");
    _assert(cache != NULL);
    stat = calloc(1, sizeof(struct stat));
    _assert(stat != NULL);
    _assert(lstat(file, stat) != -1);
    offset_cache_size = stat->st_size;
    offset_cache_blob = create_cache_blob(offset_cache_size);
    _assert(offset_cache_blob != NULL);
    fread(offset_cache_blob, offset_cache_size, 1, cache);
    import_cache_blob(offset_cache_blob);
    kernel_version_kptr = kernel_offset(kernel-version);
    _assert(KERN_POINTER_VALID(kernel_version_kptr));
    _assert(kernel_offset(cache-version) == OFFSET_CACHE_VERSION);
    ret = true;
out:;
    if (cache != NULL) fclose(cache); cache = NULL;
    SafeFreeNULL(stat);
    SafeFreeNULL(offset_cache_blob);
    return ret;
}

bool kernel_os_dict_lookup(kptr_t os_dict, const char *key, kptr_t *val) {
    bool ret = false;
    struct {
        kptr_t key;
        kptr_t value;
    } *os_dict_entries = NULL;
    _assert(KERN_POINTER_VALID(os_dict));
    _assert(key != NULL);
    _assert(val != NULL);
    uint32_t os_dict_cnt = OSDictionary_ItemCount(os_dict);
    _assert(os_dict_cnt != 0);
    kptr_t os_dict_entry_kptr = ReadKernel64(os_dict + 0x20);
    _assert(KERN_POINTER_VALID(os_dict_entry_kptr));
    os_dict_entries = calloc(os_dict_cnt, sizeof(*os_dict_entries));
    _assert(os_dict_entries != NULL);
    _assert(rkbuffer(os_dict_entry_kptr, os_dict_entries, os_dict_cnt * sizeof(*os_dict_entries)));
    for (int i = 0; i < os_dict_cnt; i++) {
        kptr_t str_kptr = os_dict_entries[i].key;
        _assert(KERN_POINTER_VALID(str_kptr));
        kptr_t val_kptr = os_dict_entries[i].value;
        _assert(KERN_POINTER_VALID(val_kptr));
        char *str = OSString_CopyString(str_kptr);
        _assert(str != NULL);
        int cmp = strcmp(str, key);
        SafeFreeNULL(str);
        if (cmp != 0) continue;
        *val = val_kptr;
        break;
    }
    ret = true;
out:;
    SafeFreeNULL(os_dict_entries);
    return ret;
}

bool kernel_set_generator(const char *generator) {
    bool ret = false;
    io_service_t appleMobileApNonce = MACH_PORT_NULL;
    io_connect_t appleMobileApNonceConnection = MACH_PORT_NULL;
    io_service_t IODTNVRAM = MACH_PORT_NULL;
    IODTNVRAM = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IODTNVRAM"));
    _assert(MACH_PORT_VALID(IODTNVRAM));
    kptr_t IODTNVRAMPort = get_address_of_port(proc_struct_addr(), IODTNVRAM);
    _assert(KERN_POINTER_VALID(IODTNVRAMPort));
    kptr_t IODTNVRAMObject = ReadKernel64(IODTNVRAMPort + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT));
    _assert(KERN_POINTER_VALID(IODTNVRAMObject));
    kptr_t IODTNVRAMDictionary = ReadKernel64(IODTNVRAMObject + 0xC0);
    _assert(KERN_POINTER_VALID(IODTNVRAMDictionary));
    kptr_t OSString = KPTR_NULL;
    _assert(kernel_os_dict_lookup(IODTNVRAMDictionary, "com.apple.System.boot-nonce", &OSString));
    if (OSString == KPTR_NULL) {
        LOG("Generating system boot nonce...");
        appleMobileApNonce = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleMobileApNonce"));
        _assert(MACH_PORT_VALID(appleMobileApNonce));
        static const uint32_t K_APPLE_MOBILE_AP_NONCE_GENERATE_NONCE = 0xC8;
        _assert(IOServiceOpen(appleMobileApNonce, mach_task_self(), 0, &appleMobileApNonceConnection) == KERN_SUCCESS);
        _assert(MACH_PORT_VALID(appleMobileApNonceConnection));
        uint8_t nonce[CC_SHA384_DIGEST_LENGTH];
        size_t nonce_size = sizeof(nonce);
        _assert(IOConnectCallStructMethod(appleMobileApNonceConnection, K_APPLE_MOBILE_AP_NONCE_GENERATE_NONCE, NULL, 0, nonce, &nonce_size) == KERN_SUCCESS);
        _assert(kernel_os_dict_lookup(IODTNVRAMDictionary, "com.apple.System.boot-nonce", &OSString));
    }
    _assert(KERN_POINTER_VALID(OSString));
    char *copiedGenerator = OSString_CopyString(OSString);
    _assert(copiedGenerator != NULL);
    LOG("System Boot Nonce Generator: %s", copiedGenerator);
    int cmp = strcmp(copiedGenerator, generator);
    SafeFreeNULL(copiedGenerator);
    if (cmp != 0) {
        kptr_t OSString_CString = OSString_CStringPtr(OSString);
        _assert(KERN_POINTER_VALID(OSString_CString));
        _assert(wkbuffer(OSString_CString, (void *)generator, strlen(generator) + 1));
        extern kern_return_t IORegistryEntrySetCFProperty(io_registry_entry_t, CFStringRef, CFTypeRef);
        _assert(IORegistryEntrySetCFProperty(IODTNVRAM, CFSTR("temp_key"), CFSTR("temp_value")) == KERN_SUCCESS);
        _assert(IORegistryEntrySetCFProperty(IODTNVRAM, CFSTR(kIONVRAMDeletePropertyKey), CFSTR("temp_key")) == KERN_SUCCESS);
        _assert(IORegistryEntrySetCFProperty(IODTNVRAM, CFSTR(kIONVRAMForceSyncNowPropertyKey), CFSTR(kBootNoncePropertyKey)) == KERN_SUCCESS);
        copiedGenerator = OSString_CopyString(OSString);
        _assert(copiedGenerator != NULL);
        LOG("New System Boot Nonce Generator: %s", copiedGenerator);
        SafeFreeNULL(copiedGenerator);
    }
    ret = true;
out:;
    if (MACH_PORT_VALID(appleMobileApNonce))
        IOObjectRelease(appleMobileApNonce);
    appleMobileApNonce = MACH_PORT_NULL;
    if (MACH_PORT_VALID(appleMobileApNonceConnection))
        IOServiceClose(appleMobileApNonceConnection);
    appleMobileApNonceConnection = MACH_PORT_NULL;
    if (MACH_PORT_VALID(IODTNVRAM))
        IOServiceClose(IODTNVRAM);
    IODTNVRAM = MACH_PORT_NULL;
    return ret;
}

bool find_kernel_offsets(void (^unrestrict)(void), void (^rerestrict)(void)) {
    init_function();
    bool ret = false;
    CFURLRef homeDirectoryURL = NULL;
    CFStringRef homeDirectory = NULL;
    CFStringRef documentsDirectory = NULL;
    CFStringRef CFKernelFile = NULL;
    CFStringRef CFCacheFile = NULL;
    const char *kernel_file = NULL;
    const char *cache_file = NULL;
    struct utsname *utsname = NULL;
    offset_entry_t *np = NULL;
    struct cache_blob *cache_blob = NULL;
    struct offset_cache *offset_cache = NULL;
    kptr_t kernel_version_kptr = KPTR_NULL;
    if (found_offsets) {
        auth_ptrs = kernel_offset(auth_ptrs);
        monolithic_kernel = kernel_offset(monolithic_kernel);
        no_kernel_execution = kernel_offset(no_kernel_execution);
        goto add_slide;
    }
    homeDirectoryURL = CFCopyHomeDirectoryURL();
    _assert(homeDirectoryURL != NULL);
    homeDirectory = CFURLCopyFileSystemPath(homeDirectoryURL, kCFURLPOSIXPathStyle);
    _assert(homeDirectory != NULL);
    documentsDirectory = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@/Documents"), homeDirectory);
    _assert(documentsDirectory != NULL);
    CFKernelFile = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@/kernelcache.dec"), documentsDirectory);
    _assert(CFKernelFile != NULL);
    CFCacheFile = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%@/offset-cache.data"), documentsDirectory);
    _assert(CFCacheFile != NULL);
    kernel_file = CFStringGetCStringPtr(CFKernelFile, kCFStringEncodingUTF8);
    _assert(kernel_file != NULL);
    cache_file = CFStringGetCStringPtr(CFCacheFile, kCFStringEncodingUTF8);
    _assert(cache_file != NULL);
    utsname = calloc(1, sizeof(struct utsname));
    _assert(utsname != NULL);
    _assert(uname(utsname) == 0);
    if (restore_offset_cache(cache_file)) {
        auth_ptrs = kernel_offset(auth_ptrs);
        monolithic_kernel = kernel_offset(monolithic_kernel);
        no_kernel_execution = kernel_offset(no_kernel_execution);
        found_offsets = true;
        goto add_slide;
    }
	if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0) {
		if (!KERN_POINTER_VALID(kernel_base)) {
			kernel_base = find_kernel_base();
			_assert(KERN_POINTER_VALID(kernel_base));
		}
		_assert(init_kernel(kread, kernel_base, NULL) == 0);
		kernel_version_kptr = find_str(utsname->version, 1, string_base_const, false, false);
		_assert(KERN_POINTER_VALID(kernel_version_kptr));
		kernel_version_kptr -= (kernel_base - STATIC_KERNEL_BASE_ADDRESS);
	} else {
		if (sandbox_check(getpid(), "file-read-data", SANDBOX_FILTER_PATH | SANDBOX_CHECK_NO_REPORT, KERNELCACHE) != 0) {
			unrestrict();
			_assert(sandbox_check(getpid(), "file-read-data", SANDBOX_FILTER_PATH | SANDBOX_CHECK_NO_REPORT, KERNELCACHE) == 0);
		}
		for (int i = 0; i < 5; i++) {
			if (access(kernel_file, F_OK | R_OK) != 0) {
				FILE *input = NULL;
				FILE *output = NULL;
				input = fopen(KERNELCACHE, "rb");
				if (input == NULL) goto close;
				output = fopen(kernel_file, "w+b");
				if (output == NULL) goto close;
				if (decompress_kernel(input, output, NULL, true) != 0) goto close;
			close:;
				if (input != NULL) fclose(input); input = NULL;
				if (output != NULL) fclose(output); output = NULL;
			}
			if (access(kernel_file, F_OK | R_OK) != 0) goto retry;
			if (init_kernel(NULL, KPTR_NULL, kernel_file) != 0) goto retry;
			kernel_version_kptr = find_str(utsname->version, 1, string_base_const, false, false);
			if (!KERN_POINTER_VALID(kernel_version_kptr)) goto retry;
			break;
		retry:;
			term_kernel();
			if (access(kernel_file, F_OK | R_OK) == 0) unlink(kernel_file);
		}
		if (unrestrict != NULL) {
			_assert(rerestrict != NULL);
			rerestrict();
		}
	}
#define _find_offset(x) do { \
    extern kptr_t find_ ## x (void); \
    kptr_t kernel_offset = find_ ## x (); \
    _assert(KERN_POINTER_VALID(kernel_offset)); \
	if (KERN_POINTER_VALID(kernel_base)) \
		kernel_offset -= (kernel_base - STATIC_KERNEL_BASE_ADDRESS); \
    set_kernel_offset(x, kernel_offset); \
} while(false)
#define _find_exec_offset(x) do { \
    extern kptr_t find_ ## x (void); \
    kptr_t kernel_offset = find_ ## x (); \
    if (KERN_POINTER_VALID(kernel_offset)) { \
        set_kernel_offset(x, kernel_offset); \
        break; \
    } \
    no_kernel_execution = true; \
} while(false)
#define _find_offsetof(x) do { \
    extern size_t offsetof_ ## x (void); \
    size_t kernel_offsetof = offsetof_ ## x (); \
    _assert(kernel_offsetof != -1); \
    set_kernel_offsetof(x, kernel_offsetof); \
} while(false)
#define _find_sizeof(x) do { \
    extern size_t size_ ## x (void); \
    size_t kernel_sizeof = size_ ## x (); \
    _assert(kernel_sizeof != -1); \
    set_kernel_size(x, kernel_sizeof); \
} while(false)
    set_kernel_offset(cache-version, OFFSET_CACHE_VERSION);
    set_kernel_offset(kernel-version, kernel_version_kptr);
    set_kernel_offset(auth_ptrs, auth_ptrs);
    set_kernel_offset(monolithic_kernel, monolithic_kernel);
    _find_offset(kernproc);
	_find_offset(cs_blob_generation_count);
    _find_offset(zone_map);
    set_kernel_offset(zone_map_ref, kernel_offset(zone_map));
    _find_offset(allproc);
	if (kCFCoreFoundationVersionNumber < kCFCoreFoundationVersionNumber_iOS_13_0) {
		_find_offset(kernel_task);
		_find_offset(shenanigans);
		if (auth_ptrs) {
			_find_offset(pmap_load_trust_cache);
			_find_exec_offset(paciza_pointer__l2tp_domain_module_start);
			_find_exec_offset(paciza_pointer__l2tp_domain_module_stop);
			_find_exec_offset(l2tp_domain_inited);
			_find_exec_offset(sysctl__net_ppp_l2tp);
			_find_exec_offset(sysctl_unregister_oid);
			_find_exec_offset(mov_x0_x4__br_x5);
			_find_exec_offset(mov_x9_x0__br_x1);
			_find_exec_offset(mov_x10_x3__br_x6);
			_find_exec_offset(kernel_forge_pacia_gadget);
			_find_exec_offset(kernel_forge_pacda_gadget);
			_find_exec_offset(IOUserClient__vtable);
			_find_exec_offset(IORegistryEntry__getRegistryEntryID);
			_find_offset(pmap_cs_cd_register);
			_find_offset(pmap_cs_cd_unregister);
		} else {
			_find_exec_offset(add_x0_x0_0x40_ret);
		}
		_find_offset(vfs_context_current);
		_find_offset(vnode_lookup);
		_find_offset(vnode_put);
		_find_offset(vn_getpath);
		if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_0) {
			_find_offset(vnode_get_snapshot);
			_find_offset(fs_lookup_snapshot_metadata_by_name_and_return_name);
			_find_offset(apfs_jhash_getvnode);
			_find_offset(cs_find_md);
			_find_offset(cs_validate_csblob);
			_find_offset(kalloc_canblock);
			_find_offset(ubc_cs_blob_allocate_site);
			_find_offset(kfree);
			_find_offset(kernel_memory_allocate);
			_find_offset(csblob_entitlements_dictionary_set);
		}
		_find_offset(trustcache);
		_find_offset(proc_find);
		_find_offset(proc_rele);
		_find_offset(ipc_space_kernel);
		_find_offset(kernel_map);
		_find_offset(vnode_getattr);
		_find_offset(OSBoolean_True);
		_find_offset(osunserializexml);
		_find_offset(smalloc);
		_find_offset(lck_mtx_lock);
		_find_offset(lck_mtx_unlock);
		_find_offset(extension_create_file);
		_find_offset(extension_add);
		_find_offset(extension_release);
		_find_offset(sfree);
		_find_offset(sstrdup);
		_find_offset(strlen);
		_find_offset(issue_extension_for_mach_service);
		_find_offset(issue_extension_for_absolute_path);
		_find_offset(IOMalloc);
		_find_offset(IOFree);
		_find_offset(socketops);
		_find_offset(realhost);
		_find_offset(platform_profile);
		_find_sizeof(sbops);
		const char **sbops = list_sbops();
		_assert(sbops != NULL);
		size_t index = 0;
		for (const char **list = sbops; *list; list++) {
			set_offset(*list, (uint64_t)index);
			index++;
		}
		SafeFreeNULL(sbops);
	}
    set_kernel_offset(no_kernel_execution, no_kernel_execution);
#undef _find_offset
#undef _find_exec_offset
#undef _find_offsetof
#undef _find_sizeof
    found_offsets = true;
    _assert(dump_offset_cache(cache_file));
add_slide:;
    if (!have_kmem_write()) {
        ret = true;
        goto out;
    }
    if (!KERN_POINTER_VALID(kernel_base)) kernel_base = find_kernel_base();
    _assert(KERN_POINTER_VALID(kernel_base));
    if (kernel_slide != -1) goto set;
    kernel_slide = kernel_base - STATIC_KERNEL_BASE_ADDRESS;
    _assert(export_cache_blob(&cache_blob) > 0);
    offset_cache = &cache_blob->cache;
    TAILQ_FOREACH(np, offset_cache, entries) {
        kptr_t kernel_offset = get_offset(np->name);
        if (KERN_POINTER_VALID(kernel_offset)) {
            kernel_offset += kernel_slide;
        }
        set_offset(np->name, kernel_offset);
    }
set:;
    if (auth_ptrs) {
        pmap_load_trust_cache = _pmap_load_trust_cache;
        LOG("Detected authentication pointers.");
    }
    if (monolithic_kernel) {
        LOG("Detected monolithic kernel.");
    }
    offset_options = kernel_offset(unrestrict-options);
    if (!KERN_POINTER_VALID(offset_options)) {
        offset_options = kmem_alloc(sizeof(uint64_t));
        _assert(KERN_POINTER_VALID(offset_options));
        _assert(WriteKernel64(offset_options, KPTR_NULL));
        set_kernel_offset(unrestrict-options, offset_options);
    }
	offset_features = kernel_offset(jailbreak-features);
    if (!KERN_POINTER_VALID(offset_features)) {
        offset_features = kmem_alloc(sizeof(uint64_t));
        _assert(KERN_POINTER_VALID(offset_features));
        _assert(WriteKernel64(offset_features, KPTR_NULL));
        set_kernel_offset(jailbreak-features, offset_features);
    }
    set_kernel_offset(kernel_slide, kernel_slide);
    ret = true;
out:;
    CFSafeReleaseNULL(homeDirectoryURL);
    CFSafeReleaseNULL(homeDirectory);
    CFSafeReleaseNULL(documentsDirectory);
    CFSafeReleaseNULL(CFKernelFile);
    CFSafeReleaseNULL(CFCacheFile);
    term_kernel();
    SafeFreeNULL(utsname);
    SafeFreeNULL(cache_blob);
    return ret;
}

bool kernel_forge_pmap_cs_entries_internal(kptr_t csblobs) {
    bool ret = false;
#if __arm64e__
    _assert(KERN_POINTER_VALID(csblobs));
start:;
    int64_t csb_base_offset = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_base_offset));
    if (csb_base_offset == 0x4141414141414141) goto next;
    kptr_t pmap_cs_entry = ReadKernel64(csblobs + offsetof(struct cs_blob, pmap_cs_entry));
    uint32_t trust_level = 0;
    if (pmap_cs_entry != KPTR_NULL) {
        trust_level = ReadKernel32(pmap_cs_entry + koffset(KSTRUCT_OFFSET_PMAP_CS_CODE_DIRECTORY_TRUST_LEVEL));
        if (trust_level < 2) goto next;
    }
    uint64_t csb_mem_size = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_mem_size));
    if (csb_mem_size == 0) goto next;
    kptr_t csb_mem_kaddr = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_mem_kaddr));
    kptr_t csb_cd = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_cd));
    uint64_t csb_cd_offset = csb_cd - csb_mem_kaddr;
    uint64_t new_csb_mem_size = csb_mem_size;
    uint64_t new_csb_mem_kernel_size = new_csb_mem_size;
    new_csb_mem_kernel_size = (vm_kernel_page_mask + new_csb_mem_kernel_size) & ~vm_kernel_page_mask;
    kptr_t new_csb_mem_kaddr = KPTR_NULL;
    kern_return_t kr = kernel_memory_allocate(_kernel_map, (vm_offset_t *)&new_csb_mem_kaddr, new_csb_mem_kernel_size, 0, KMA_KOBJECT, VM_KERN_MEMORY_SECURITY);
    if (kr != KERN_SUCCESS) goto next;
    kmemcpy(new_csb_mem_kaddr, csb_mem_kaddr, csb_mem_size);
    struct cs_blob cs_blob;
    bzero(&cs_blob, sizeof(cs_blob));
    cs_blob.csb_next = (struct cs_blob *)ReadKernel64(csblobs + offsetof(struct cs_blob, csb_next));
    cs_blob.csb_mem_kaddr = new_csb_mem_kaddr;
    cs_blob.csb_mem_size = new_csb_mem_kernel_size;
    cs_blob.csb_base_offset = 0x4141414141414141;
    cs_blob.csb_end_offset = 0x8000;
    vm_size_t kalloc_size = sizeof(cs_blob);
    kptr_t cs_blob_kptr = kalloc_canblock(&kalloc_size, true, _ubc_cs_blob_allocate_site);
    wkbuffer(cs_blob_kptr, &cs_blob, sizeof(cs_blob));
    WriteKernel64(csblobs + offsetof(struct cs_blob, csb_next), cs_blob_kptr);
    kptr_t new_csb_cd = new_csb_mem_kaddr + csb_cd_offset;
    WriteKernel32(new_csb_cd + offsetof(CS_CodeDirectory, version), 0x30200);
    WriteKernel64(new_csb_cd + offsetof(CS_CodeDirectory, execSegFlags), 0);
    WriteKernel64(new_csb_cd + offsetof(CS_CodeDirectory, execSegBase), 0);
    WriteKernel64(new_csb_cd + offsetof(CS_CodeDirectory, execSegLimit), 0);
    kptr_t pmap_cs_entry_kptr = kmem_alloc(sizeof(kptr_t));
    WriteKernel64(pmap_cs_entry_kptr, KPTR_NULL);
    kptr_t pmap_cs_cd_register_kptr = kernel_offset(pmap_cs_cd_register);
    kr = (kern_return_t)kexec(pmap_cs_cd_register_kptr, new_csb_mem_kaddr, new_csb_mem_kernel_size, csb_cd_offset, -1, pmap_cs_entry_kptr, KPTR_NULL, KPTR_NULL);
    kptr_t new_pmap_cs_entry = ReadKernel64(pmap_cs_entry_kptr);
    kmem_free(pmap_cs_entry_kptr, sizeof(kptr_t));
    if (kr != KERN_SUCCESS) goto next;
    WriteKernel64(csblobs + offsetof(struct cs_blob, pmap_cs_entry), new_pmap_cs_entry);
    trust_level = ReadKernel32(new_pmap_cs_entry + koffset(KSTRUCT_OFFSET_PMAP_CS_CODE_DIRECTORY_TRUST_LEVEL));
    if (trust_level != 1) goto next;
    uint32_t ref_count = 0;
    if (pmap_cs_entry != KPTR_NULL) {
        ref_count = ReadKernel32(pmap_cs_entry + koffset(KSTRUCT_OFFSET_PMAP_CS_CODE_DIRECTORY_REGISTERED));
    }
    if (ref_count > 0) {
        struct cs_blob cs_blob;
        bzero(&cs_blob, sizeof(cs_blob));
        cs_blob.csb_next = (struct cs_blob *)ReadKernel64(csblobs + offsetof(struct cs_blob, csb_next));
        cs_blob.pmap_cs_entry = (void *)pmap_cs_entry;
        cs_blob.csb_base_offset = 0x4141414141414141;
        cs_blob.csb_end_offset = 0x8000;
        vm_size_t kalloc_size = sizeof(cs_blob);
        kptr_t cs_blob_kptr = kalloc_canblock(&kalloc_size, true, _ubc_cs_blob_allocate_site);
        wkbuffer(cs_blob_kptr, &cs_blob, sizeof(cs_blob));
        WriteKernel64(csblobs + offsetof(struct cs_blob, csb_next), cs_blob_kptr);
    } else if (pmap_cs_entry != KPTR_NULL) {
        kptr_t pmap_cs_cd_unregister_kptr = kernel_offset(pmap_cs_cd_unregister);
        _assert(KERN_POINTER_VALID(pmap_cs_cd_unregister_kptr));
        kr = (kern_return_t)kexec(pmap_cs_cd_unregister_kptr, pmap_cs_entry, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL, KPTR_NULL);
        if (kr != KERN_SUCCESS) goto next;
    }
next:;
    csblobs = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_next));
    if (KERN_POINTER_VALID(csblobs)) goto start;
#endif
    ret = true;
out:;
    return ret;
}

bool kernel_forge_pmap_cs_entries(const char *path) {
    bool ret = false;
    int fd = 0;
#if __arm64e__
    fd = open(path, O_RDONLY);
    _assert(fd > 0);
    kptr_t vnode = find_vnode_with_fd(proc_struct_addr(), fd);
    _assert(KERN_POINTER_VALID(vnode));
    _assert(kernel_forge_pmap_cs_entries_internal(vnode));
#endif
    ret = true;
out:;
    if (fd > 0) close(fd);
    return ret;
}

bool set_memory_object_code_signed(kptr_t ubcinfo) {
    bool ret = false;
    _assert(KERN_POINTER_VALID(ubcinfo));
    kptr_t ui_control = ReadKernel64(ubcinfo + offsetof(struct ubc_info, ui_control));
    _assert(KERN_POINTER_VALID(ui_control));
    kptr_t moc_object = ReadKernel64(ui_control + sizeof(kptr_t));
    _assert(KERN_POINTER_VALID(moc_object));
    uint32_t offset = 0xb8;
    if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_0) {
        offset -= 0x10;
    }
    uint32_t flags = ReadKernel32(moc_object + offset);
    flags |= 0x100;
    _assert(WriteKernel32(moc_object + offset, flags));
    ret = true;
out:;
    return ret;
}

bool set_csb_entitlements(void) {
    bool ret = false;
    kptr_t csb_entitlements = KPTR_NULL;
    csb_entitlements = OSUnserializeXML(DEFAULT_ENTITLEMENTS);
    _assert(KERN_POINTER_VALID(csb_entitlements));
    set_kernel_offset(csb_entitlements, csb_entitlements);
    ret = true;
out:;
    return ret;
}

bool set_csb_entitlements_blob(void) {
    bool ret = false;
    uint32_t size = sizeof(uint32_t) + sizeof(uint32_t) + strlen(DEFAULT_ENTITLEMENTS);
    CS_GenericBlob *entitlements_blob = (CS_GenericBlob *)calloc(1, size);
    _assert(entitlements_blob != NULL);
    entitlements_blob->magic = CSMAGIC_EMBEDDED_ENTITLEMENTS;
    entitlements_blob->length = size;
    strncpy(entitlements_blob->data, DEFAULT_ENTITLEMENTS, strlen(DEFAULT_ENTITLEMENTS) + 1);
    vm_size_t kalloc_size = size;
    kptr_t csb_entitlements_blob = kalloc_canblock(&kalloc_size, true, _ubc_cs_blob_allocate_site);
    wkbuffer(csb_entitlements_blob, entitlements_blob, size);
    SafeFreeNULL(entitlements_blob);
    set_kernel_offset(csb_entitlements_blob, csb_entitlements_blob);
    ret = true;
out:;
    return ret;
}

bool kernel_forge_csblobs_physical(const char *path) {
    bool ret = false;
    img_info_t physical;
    bzero(&physical, sizeof(physical));
    char virtual_path[PROC_PIDPATHINFO_MAXSIZE];
    bzero(virtual_path, sizeof(virtual_path));
    img_info_t virtual;
    bzero(&virtual, sizeof(virtual));
    _assert(path != NULL);
    physical.name = path;
    physical.file_off = get_mach_offset(path);
    _assert(open_img(&physical) == 0);
    uint32_t physical_cs_size = 0;
    const void *physical_cs = find_code_signature(&physical, &physical_cs_size);
    _assert(physical_cs != NULL);
    const CS_CodeDirectory *physical_cd = NULL;
    uint32_t physical_csb_offset = 0;
    const CS_GenericBlob *physical_entitlements = NULL;
    uint32_t physical_entitlements_offset = 0;
    _assert(find_best_codedir(physical_cs, physical_cs_size, &physical_cd, &physical_csb_offset, &physical_entitlements, &physical_entitlements_offset) == 0);
    uint8_t physical_hash[CS_CDHASH_LEN];
    bzero(physical_hash, sizeof(physical_hash));
    _assert(hash_code_directory(physical_cd, physical_hash) == 0);
    snprintf(virtual_path, sizeof(virtual_path), "/Library/Caches/cy-%X", ntohl(*(uint64_t *)physical_hash));
    _assert(copyfile(path, virtual_path, 0, COPYFILE_ALL) == 0);
    pid_t pid = 0;
    char **argv = ARRAY(char *, "/usr/libexec/ldid_wrapper", "-M", "-S/usr/share/entitlements/inject.xml", virtual_path, NULL);
    _assert(posix_spawn(&pid, argv[0], NULL, NULL, argv, NULL) == 0);
    int xstat = 0;
    _assert(waitpid(pid, &xstat, 0) != -1);
    _assert(WEXITSTATUS(xstat) == 0);
    virtual.name = virtual_path;
    virtual.file_off = get_mach_offset(virtual_path);
    _assert(open_img(&virtual) == 0);
    uint32_t virtual_cs_size = 0;
    const void *virtual_cs = find_code_signature(&virtual, &virtual_cs_size);
    _assert(virtual_cs != NULL);
    const CS_CodeDirectory *virtual_cd = NULL;
    uint32_t virtual_csb_offset = 0;
    const CS_GenericBlob *virtual_entitlements = NULL;
    uint32_t virtual_entitlements_offset = 0;
    _assert(find_best_codedir(virtual_cs, virtual_cs_size, &virtual_cd, &virtual_csb_offset, &virtual_entitlements, &virtual_entitlements_offset) == 0);
    uint8_t virtual_hash[CS_CDHASH_LEN];
    bzero(virtual_hash, sizeof(virtual_hash));
    _assert(hash_code_directory(virtual_cd, virtual_hash) == 0);
    fsignatures_t virtual_signature;
    bzero(&virtual_signature, sizeof(virtual_signature));
    virtual_signature.fs_file_start = virtual.size;
    virtual_signature.fs_blob_start = (void *)virtual_cs;
    virtual_signature.fs_blob_size = virtual_cs_size;
    _assert(fcntl(physical.fd, F_ADDSIGS, &virtual_signature) != -1);
    ret = true;
out:;
    close_img(&physical);
    close_img(&virtual);
    return ret;
}

bool kernel_forge_csblobs_virtual(const char *path) {
    bool ret = false;
    kptr_t cs_blob_kptr = KPTR_NULL;
    struct cs_blob cs_blob;
    bzero(&cs_blob, sizeof(cs_blob));
    kptr_t vnode = KPTR_NULL;
    img_info_t info;
    bzero(&info, sizeof(info));
    struct stat st;
    bzero(&st, sizeof(st));
    const CS_CodeDirectory *blob = NULL;
    uint32_t blob_offset = 0;
    const CS_GenericBlob *entitlements = NULL;
    uint32_t entitlements_offset = 0;
    kptr_t cs_kptr = KPTR_NULL;
    _assert(stat(path, &st) != -1);
    vnode = get_vnode_for_path(path);
    _assert(KERN_POINTER_VALID(vnode));
    uint16_t v_type = 0;
    _assert(rkbuffer(vnode + offsetof(struct vnode_struct, v_type), &v_type, sizeof(v_type)));
    _assert(v_type == VREG);
    kptr_t ubcinfo = ReadKernel64(vnode + koffset(KSTRUCT_OFFSET_VNODE_V_UBCINFO));
    _assert(KERN_POINTER_VALID(ubcinfo));
    kptr_t csblobs = ReadKernel64(ubcinfo + koffset(KSTRUCT_OFFSET_UBC_INFO_CSBLOBS));
    if (KERN_POINTER_VALID(csblobs)) goto revalidate;
    uint32_t macho_offset = get_mach_offset(path);
    _assert(macho_offset != -1);
    info.name = path;
    info.file_off = macho_offset;
    _assert(open_img(&info) == 0);
    uint32_t cs_size = 0;
    const void *cs = find_code_signature(&info, &cs_size);
    _assert(cs != NULL);
    _assert(find_best_codedir(cs, cs_size, &blob, &blob_offset, &entitlements, &entitlements_offset) == 0);
    uint8_t cdhash[CS_CDHASH_LEN];
    bzero(cdhash, sizeof(cdhash));
    _assert(hash_code_directory(blob, cdhash) == 0);
    int64_t cs_kernel_size = 0;
    if (cs_size > pmap_cs_blob_limit) {
        cs_kernel_size = (vm_kernel_page_mask + cs_size) & ~vm_kernel_page_mask;
        int rv = kernel_memory_allocate(_kernel_map, (vm_offset_t *)&cs_kptr, cs_kernel_size, 0, KMA_KOBJECT, VM_KERN_MEMORY_SECURITY);
        _assert(rv == KERN_SUCCESS);
    } else {
        cs_kernel_size = cs_size;
        vm_size_t kalloc_size = cs_kernel_size;
        cs_kptr = kalloc_canblock(&kalloc_size, true, _ubc_cs_blob_allocate_site);
        _assert(KERN_POINTER_VALID(cs_kptr));
    }
    _assert(wkbuffer(cs_kptr, (void *)cs, cs_size));
    cs_blob.csb_next = NULL;
    cs_blob.csb_cpu_type = CPU_TYPE_ARM64;
    cs_blob.csb_flags = (ntohl(blob->flags) & CS_ALLOWED_MACHO) | CS_VALID | CS_SIGNED;
    cs_blob.csb_base_offset = macho_offset;
    cs_blob.csb_start_offset = 0;
    if (ntohl(blob->version) >= CS_SUPPORTSSCATTER && ntohl(blob->scatterOffset) != 0) {
        const SC_Scatter *scatter = (const SC_Scatter *)((const char *)blob + ntohl(blob->scatterOffset));
        _assert(scatter != NULL);
        cs_blob.csb_start_offset = ((off_t)ntohl(scatter->base)) * (1U << blob->pageSize);
    }
    cs_blob.csb_mem_size = cs_kernel_size;
    cs_blob.csb_mem_offset = 0;
    cs_blob.csb_mem_kaddr = cs_kptr;
    memcpy(cs_blob.csb_cdhash, cdhash, CS_CDHASH_LEN);
    kptr_t csb_hashtype = cs_find_md(blob->hashType);
    _assert(KERN_POINTER_VALID(csb_hashtype));
    cs_blob.csb_hashtype = (const struct cs_hash *)csb_hashtype;
    cs_blob.csb_hash_pagesize = (1U << blob->pageSize);
    cs_blob.csb_hash_pagemask = (1U << blob->pageSize) - 1;
    cs_blob.csb_hash_pageshift = blob->pageSize;
    cs_blob.csb_end_offset = ntohl(blob->codeLimit) + cs_blob.csb_hash_pagemask & ~cs_blob.csb_hash_pagemask;
    cs_blob.csb_hash_firstlevel_pagesize = 0;
    cs_blob.csb_cd = (const CS_CodeDirectory *)(cs_kptr + blob_offset);
    if (cs_blob.csb_flags & CS_PLATFORM_BINARY) {
        cs_blob.csb_platform_binary = true;
        cs_blob.csb_platform_path = !!(cs_blob.csb_flags & CS_PLATFORM_PATH);
    } else if ((ntohl(blob->version) >= CS_SUPPORTSTEAMID) && (blob->teamOffset > 0)) {
        kptr_t teamid_kptr = cs_kptr + ntohl(blob->teamOffset);
        _assert(KERN_POINTER_VALID(teamid_kptr));
        cs_blob.csb_teamid = (const char *)teamid_kptr;
    }
    cs_blob.csb_reconstituted = true;
    vm_size_t kalloc_size = sizeof(cs_blob);
    cs_blob_kptr = kalloc_canblock(&kalloc_size, true, _ubc_cs_blob_allocate_site);
    _assert(KERN_POINTER_VALID(cs_blob_kptr));
    _assert(wkbuffer(cs_blob_kptr, &cs_blob, sizeof(cs_blob)));
    _assert(WriteKernel64(ubcinfo + koffset(KSTRUCT_OFFSET_UBC_INFO_CSBLOBS), cs_blob_kptr));
    csblobs = cs_blob_kptr;
    cs_blob_kptr = KPTR_NULL;
    _assert(set_memory_object_code_signed(ubcinfo));
    _assert(wkbuffer(ubcinfo + offsetof(struct ubc_info, cs_mtime), &st.st_mtimespec, sizeof(st.st_mtimespec)));
revalidate:;
    kptr_t csb_entitlements = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_entitlements));
    if (csb_entitlements == KPTR_NULL) {
        csb_entitlements = entitlements != NULL ? OSUnserializeXML(entitlements->data) : kernel_offset(csb_entitlements);
        _assert(KERN_POINTER_VALID(csb_entitlements));
        OSObject_Retain(csb_entitlements);
        _assert(WriteKernel64(csblobs + offsetof(struct cs_blob, csb_entitlements), csb_entitlements));
    }
    if (entitlements != NULL) {
        OSDictionary_SetItem(csb_entitlements, "platform-application", OSBoolTrue);
        OSDictionary_SetItem(csb_entitlements, "get-task-allow", OSBoolTrue);
        OSDictionary_SetItem(csb_entitlements, "com.apple.private.skip-library-validation", OSBoolTrue);
        if (strncmp(path, "/Applications/", strlen("/Applications/")) == 0) {
            OSDictionary_SetItem(csb_entitlements, "com.apple.private.security.no-container", OSBoolTrue);
        }
    }
    kptr_t csb_entitlements_blob = ReadKernel64(csblobs + offsetof(struct cs_blob, csb_entitlements_blob));
    if (csb_entitlements_blob == KPTR_NULL) {
        csb_entitlements_blob = entitlements != NULL ? cs_kptr + entitlements_offset : kernel_offset(csb_entitlements_blob);
        _assert(KERN_POINTER_VALID(csb_entitlements_blob));
        _assert(WriteKernel64(csblobs + offsetof(struct cs_blob, csb_entitlements_blob), csb_entitlements_blob));
    }
    _assert(WriteKernel32(ubcinfo + offsetof(struct ubc_info, cs_add_gen), _cs_blob_generation_count));
    _assert(kernel_forge_pmap_cs_entries_internal(csblobs));
    ret = true;
out:;
    close_img(&info);
    if (KERN_POINTER_VALID(vnode)) vnode_put(vnode);
    return ret;
}

bool kernel_forge_csblobs(const char *path) {
    if (have_kernel_execution()) {
        return kernel_forge_csblobs_virtual(path);
    }
    return kernel_forge_csblobs_physical(path);
}

bool kernel_add_signatures(const char *path) {
    bool ret = false;
    int fd = 0;
    _assert(path != NULL);
    fd = open(path, O_RDONLY);
    _assert(fd > 0);
    uint32_t macho_offset = get_mach_offset(path);
    _assert(macho_offset != -1);
    img_info_t img_info;
    bzero(&img_info, sizeof(img_info));
    img_info.name = path;
    img_info.file_off = macho_offset;
    _assert(open_img(&img_info) == 0);
    uint32_t cs_length = 0;
    const void *code_signature = find_code_signature(&img_info, &cs_length);
    _assert(code_signature != NULL);
    fsignatures_t fssignature;
    bzero(&fssignature, sizeof(fssignature));
    fssignature.fs_file_start = img_info.size;
    fssignature.fs_blob_start = (void *)code_signature;
    fssignature.fs_blob_size = cs_length;
    _assert(fcntl(fd, F_ADDSIGS, &fssignature) != -1);
    ret = true;
out:;
    close_img(&img_info);
    if (fd > 0) close(fd);
    return ret;
}

bool kernel_hook_file(const char *original_file, const char *hook_file, const char *old_file) {
    bool ret = false;
    int tfd = 0;
    int hfd = 0;
    int ofd = 0;
    _assert(access(original_file, F_OK | R_OK) == 0);
    _assert(access(hook_file, F_OK | R_OK) == 0);
    tfd = open(original_file, O_RDONLY);
    _assert(tfd > 0);
    hfd = open(hook_file, O_RDONLY);
    _assert(hfd > 0);
    ofd = open(old_file, O_CREAT | O_TRUNC);
    _assert(ofd > 0);
    kptr_t tvp_kptr = find_vnode_with_fd(proc_struct_addr(), tfd);
    struct vnode_struct tvp;
    bzero(&tvp, sizeof(tvp));
    rkbuffer(tvp_kptr, &tvp, sizeof(tvp));
    kptr_t hvp_kptr = find_vnode_with_fd(proc_struct_addr(), hfd);
    struct vnode_struct hvp;
    bzero(&hvp, sizeof(hvp));
    rkbuffer(hvp_kptr, &hvp, sizeof(hvp));
    kptr_t ovp_kptr = find_vnode_with_fd(proc_struct_addr(), ofd);
    struct vnode_struct ovp;
    bzero(&ovp, sizeof(ovp));
    rkbuffer(ovp_kptr, &ovp, sizeof(ovp));
    if ((tvp.v_flag & 0x20000000) != 0 && (hvp.v_flag & 0x40000000) != 0 && (ovp.v_flag & 0x80000000) != 0) {
        ret = true;
        goto out;
    }
    struct vnode_struct tvpr;
    bzero(&tvpr, sizeof(tvpr));
    memcpy(&tvpr, &hvp, sizeof(hvp));
    tvpr.v_usecount = 0xf00d;
    tvpr.v_kusecount = tvp.v_kusecount;
    tvpr.v_parent = tvp.v_parent;
    tvpr.v_freelist = tvp.v_freelist;
    tvpr.v_mntvnodes = tvp.v_mntvnodes;
    tvpr.v_ncchildren = tvp.v_ncchildren;
    tvpr.v_nclinks = tvp.v_nclinks;
    tvpr.v_flag |= 0x20000000;
    struct vnode_struct hvpr;
    bzero(&hvpr, sizeof(hvpr));
    memcpy(&hvpr, &hvp, sizeof(hvp));
    hvpr.v_usecount = 0xf00d;
    hvpr.v_flag |= 0x40000000;
    struct vnode_struct ovpr;
    bzero(&ovpr, sizeof(ovpr));
    memcpy(&ovpr, &tvp, sizeof(tvp));
    ovpr.v_usecount = 0xf00d;
    ovpr.v_kusecount = ovp.v_kusecount;
    ovpr.v_parent = ovp.v_parent;
    ovpr.v_freelist = ovp.v_freelist;
    ovpr.v_mntvnodes = ovp.v_mntvnodes;
    ovpr.v_ncchildren = ovp.v_ncchildren;
    ovpr.v_nclinks = ovp.v_nclinks;
    ovpr.v_flag |= 0x80000000;
    wkbuffer(tvp_kptr, &tvpr, sizeof(tvpr));
    wkbuffer(hvp_kptr, &hvpr, sizeof(hvpr));
    wkbuffer(ovp_kptr, &ovpr, sizeof(ovpr));
    ret = true;
out:;
    if (tfd > 0) close(tfd);
    if (hfd > 0) close(hfd);
    if (ofd > 0) close(ofd);
    return ret;
}

bool kernel_resign_physical_object(const char *file, const char *name, const char *dylib) {
    bool ret = false;
    int fd = 0;
    int fake_fd = 0;
    uint32_t file_off = 0;
    img_info_t info;
    bzero(&info, sizeof(info));
    img_info_t dylib_info;
    bzero(&dylib_info, sizeof(dylib_info));
    uint8_t cdhash[CS_CDHASH_LEN];
    bzero(cdhash, sizeof(cdhash));
    uint8_t dylib_cdhash[CS_CDHASH_LEN];
    bzero(dylib_cdhash, sizeof(dylib_cdhash));
    fd = open(file, O_RDONLY);
    _assert(fd > 0);
    kptr_t rvp_kptr = find_vnode_with_fd(proc_struct_addr(), fd);
    _assert(KERN_POINTER_VALID(rvp_kptr));
    struct vnode_struct rvp;
    bzero(&rvp, sizeof(rvp));
    _assert(rkbuffer(rvp_kptr, &rvp, sizeof(rvp)));
    if ((rvp.v_flag & 0x20000000) != 0) {
        ret = true;
        goto out;
    }
    file_off = get_mach_offset(file);
    _assert(file_off != -1);
    info.name = file;
    info.file_off = file_off;
    _assert(open_img(&info) == 0);
    uint32_t cs_size = 0;
    const void *cs = find_code_signature(&info, &cs_size);
    _assert(cs != NULL);
    const CS_CodeDirectory *cd = NULL;
    uint32_t csb_offset = 0;
    const CS_GenericBlob *entitlements = NULL;
    uint32_t entitlements_offset = 0;
    _assert(find_best_codedir(cs, cs_size, &cd, &csb_offset, &entitlements, &entitlements_offset) == 0);
    _assert(hash_code_directory(cd, cdhash) == 0);
    if (dylib != NULL) {
        uint32_t dylib_file_off = get_mach_offset(dylib);
        _assert(dylib_file_off != -1);
        dylib_info.name = dylib;
        dylib_info.file_off = dylib_file_off;
        _assert(open_img(&dylib_info) == 0);
        uint32_t dylib_cs_size = 0;
        const void *dylib_cs = find_code_signature(&dylib_info, &dylib_cs_size);
        _assert(dylib_cs != NULL);
        const CS_CodeDirectory *dylib_cd = NULL;
        uint32_t dylib_csb_offset = 0;
        const CS_GenericBlob *dylib_entitlements = NULL;
        uint32_t dylib_entitlements_offset = 0;
        _assert(find_best_codedir(dylib_cs, dylib_cs_size, &dylib_cd, &dylib_csb_offset, &dylib_entitlements, &dylib_entitlements_offset) == 0);
        _assert(hash_code_directory(dylib_cd, dylib_cdhash) == 0);
    }
    char dir[PROC_PIDPATHINFO_MAXSIZE];
    bzero(dir, sizeof(dir));
    snprintf(dir, sizeof(dir), "/Library/Caches/%X", ntohl(*(uint64_t *)cdhash));
    struct stat dir_st;
    if (lstat(dir, &dir_st) == -1) {
        _assert(mkdir(dir, S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO) == 0);
    } else if (!S_ISDIR(dir_st.st_mode)) {
        _assert(false);
    }
    char sub_dir[PROC_PIDPATHINFO_MAXSIZE];
    bzero(sub_dir, sizeof(sub_dir));
    snprintf(sub_dir, sizeof(sub_dir), "%s/%X", dir, ntohl(*(uint64_t *)dylib_cdhash));
    struct stat sub_dir_st;
    if (lstat(sub_dir, &sub_dir_st) == -1) {
        _assert(mkdir(sub_dir, S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO) == 0);
    } else if (!S_ISDIR(sub_dir_st.st_mode)) {
        _assert(false);
    }
    char fake[PROC_PIDPATHINFO_MAXSIZE];
    bzero(fake, sizeof(fake));
    snprintf(fake, sizeof(fake), "%s/%s", sub_dir, name);
    struct stat fake_st;
    bzero(&fake_st, sizeof(fake_st));
    if (lstat(fake, &fake_st) == -1) {
        _assert(copyfile(file, fake, 0, COPYFILE_ALL) == 0);
    } else if (!S_ISREG(fake_st.st_mode)) {
        _assert(false);
    }
    if (dylib != NULL) {
        const char *inject[] = { "insert_dylib", "--all-yes", "--inplace", "--overwrite", dylib, fake };
        _assert(add_dylib(sizeof(inject) / sizeof(inject[0]), inject) == 0);
    }
    pid_t pid = 0;
    char **argv = ARRAY(char *, "/usr/libexec/ldid_wrapper", "-M", "-S/usr/share/entitlements/inject.xml", fake, NULL);
    _assert(posix_spawn(&pid, argv[0], NULL, NULL, argv, NULL) == 0);
    int xstat = 0;
    _assert(waitpid(pid, &xstat, 0) != -1);
    _assert(WEXITSTATUS(xstat) == 0);
    fake_fd = open(fake, O_RDONLY);
    _assert(fake_fd > 0);
    kptr_t fvp_kptr = find_vnode_with_fd(proc_struct_addr(), fake_fd);
    _assert(KERN_POINTER_VALID(fvp_kptr));
    struct vnode_struct fvp;
    bzero(&fvp, sizeof(fvp));
    _assert(rkbuffer(fvp_kptr, &fvp, sizeof(fvp)));
    fvp.v_usecount = 0xf00d;
    fvp.v_flag |= (VSYSTEM | VNOFLUSH | 0x20000000);
    _assert(wkbuffer(fvp_kptr, &fvp, sizeof(fvp)));
    fvp.v_kusecount = rvp.v_kusecount;
    fvp.v_parent = rvp.v_parent;
    fvp.v_freelist = rvp.v_freelist;
    fvp.v_mntvnodes = rvp.v_mntvnodes;
    fvp.v_ncchildren = rvp.v_ncchildren;
    fvp.v_nclinks = rvp.v_nclinks;
    _assert(wkbuffer(rvp_kptr, &fvp, sizeof(fvp)));
    ret = true;
out:;
    close_img(&info);
    close_img(&dylib_info);
    if (fd > 0) close(fd);
    if (fake_fd > 0) close(fake_fd);
    return ret;
}
