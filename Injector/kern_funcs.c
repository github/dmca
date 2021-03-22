#include <sys/snapshot.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <copyfile.h>
#include <spawn.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/sysctl.h>
#include <mach-o/dyld.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <CoreFoundation/CoreFoundation.h>
#include <mach/mach.h>
#include "patchfinder64.h"
#include <kmem.h>
#include "CSCommon.h"
#include "kern_funcs.h"
#include "kernel_call.h"
#include "parameters.h"
#include "kc_parameters.h"
#include "kernel_memory.h"

uint64_t kernel_base;
static mach_port_t tfp0=MACH_PORT_NULL;
size_t kread(uint64_t where, void *p, size_t size);
size_t kwrite(uint64_t where, const void *p, size_t size);

void set_tfp0(mach_port_t port) {
    tfp0 = port;
}

void wk32(uint64_t kaddr, uint32_t val) {
    kwrite(kaddr, &val, sizeof(uint32_t));
}
 
void wk64(uint64_t kaddr, uint64_t val) {
    kwrite(kaddr, &val, sizeof(uint64_t));
}
 
uint32_t rk32(uint64_t kaddr) {
    uint32_t val = 0;

    if (kread(kaddr, &val, sizeof(val)) != sizeof(val)) {
        return 0;
    }
    return val;
}
 
uint64_t rk64(uint64_t kaddr) {
    uint64_t val = 0;

    if (kread(kaddr, &val, sizeof(val)) != sizeof(val)) {
        return 0;
    }
    return val;
}
 
uint64_t kmem_alloc(uint64_t size) {
    if (tfp0 == MACH_PORT_NULL) {
        printf("attempt to allocate kernel memory before any kernel memory write primitives available\n");
        sleep(3);
        return 0;
    }
   
    kern_return_t err;
    mach_vm_address_t addr = 0;
    mach_vm_size_t ksize = round_page_kernel(size);
    err = mach_vm_allocate(tfp0, &addr, ksize, VM_FLAGS_ANYWHERE);
    if (err != KERN_SUCCESS) {
        printf("unable to allocate kernel memory via tfp0: %s %x\n", mach_error_string(err), err);
        sleep(3);
        return 0;
    }
    return addr;
}

size_t kread(uint64_t where, void *p, size_t size)
{
    int rv;
    size_t offset = 0;
    while (offset < size) {
        mach_vm_size_t sz, chunk = 2048;
        if (chunk > size - offset) {
            chunk = size - offset;
        }
        rv = mach_vm_read_overwrite(tfp0, where + offset, chunk, (mach_vm_address_t)p + offset, &sz);
        if (rv || sz == 0) {
            fprintf(stderr, "[e] error reading kernel @%p\n", (void *)(offset + where));
            break;
        }
        offset += sz;
    }
    return offset;
}
 
size_t kwrite(uint64_t where, const void *p, size_t size)
{
    int rv;
    size_t offset = 0;

    if (tfp0 == MACH_PORT_NULL) {
        printf("attempt to write to kernel memory before any kernel memory write primitives available\n");
        sleep(3);
        return offset;
    }
   
    while (offset < size) {
        size_t chunk = 2048;
        if (chunk > size - offset) {
            chunk = size - offset;
        }
        rv = mach_vm_write(tfp0, where + offset, (mach_vm_offset_t)p + offset, (mach_msg_type_number_t)chunk);
        if (rv) {
            fprintf(stderr, "[e] error writing kernel @%p\n", (void *)(offset + where));
            break;
        }
        offset += chunk;
    }
    return offset;
}

uint64_t task_self_addr() {
    uint64_t kernproc = rk64(rk64(GETOFFSET(kernel_task)) + OFFSET(task, bsd_info));
    uint64_t proc = kernproc;
    pid_t our_pid = getpid();
    uint64_t our_proc = 0;
    while (proc) {
        if (rk32(proc + OFFSET(proc, p_pid)) == our_pid) {
            our_proc = proc;
            break;
        }
        proc = rk64(proc + OFFSET(proc, p_list));
    }
    uint64_t task_addr = rk64(our_proc + OFFSET(proc, task));
    uint64_t itk_space = rk64(task_addr + OFFSET(task, itk_space));
    uint64_t is_table = rk64(itk_space + OFFSET(ipc_space, is_table));
    mach_port_t port = mach_task_self();
    uint32_t port_index = port >> 8;
    const int sizeof_ipc_entry_t = SIZE(ipc_entry);
    uint64_t port_addr = rk64(is_table + (port_index * sizeof_ipc_entry_t));
    return port_addr;
}

int (*pmap_load_trust_cache)(uint64_t kernel_trust, size_t length) = NULL;
int _pmap_load_trust_cache(uint64_t kernel_trust, size_t length) {
    return (int)kernel_call_7(GETOFFSET(pmap_load_trust_cache), 3, kernel_trust, length, 0);
}
