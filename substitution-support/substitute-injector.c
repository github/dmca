#include "substitute-injector.h"
#include <mach/mach.h>
#include <dlfcn.h>
#include <pthread.h>

extern int pthread_create_from_mach_thread(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
extern kern_return_t bootstrap_look_up(mach_port_t bp, char *name, mach_port_t *sp);
extern mach_port_t mach_reply_port(void);
extern kern_return_t mach_vm_allocate(task_t task, mach_vm_address_t *addr, mach_vm_size_t size, int flags);
extern kern_return_t mach_vm_deallocate(task_t task, mach_vm_address_t address, mach_vm_size_t size);
extern kern_return_t mach_vm_read(vm_map_t target_task, mach_vm_address_t address, mach_vm_size_t size, vm_offset_t *data, mach_msg_type_number_t *dataCnt);
extern kern_return_t mach_vm_write(vm_map_t target_task, mach_vm_address_t address, vm_offset_t data, mach_msg_type_number_t dataCnt);
extern kern_return_t mach_vm_read_overwrite(vm_map_t target_task, mach_vm_address_t address, mach_vm_size_t size, mach_vm_address_t data, mach_vm_size_t *outsize);
extern kern_return_t mach_vm_protect(task_t task, mach_vm_address_t addr, mach_vm_size_t size, boolean_t set_max, vm_prot_t new_prot);
extern kern_return_t mach_vm_map(task_t task, mach_vm_address_t *addr, mach_vm_size_t size, mach_vm_offset_t mask, int flags, mem_entry_name_port_t object, memory_object_offset_t offset, boolean_t copy, vm_prot_t cur, vm_prot_t max, vm_inherit_t inheritance);
extern kern_return_t mach_vm_remap(vm_map_t dst, mach_vm_address_t *dst_addr, mach_vm_size_t size, mach_vm_offset_t mask, int flags, vm_map_t src, mach_vm_address_t src_addr, boolean_t copy, vm_prot_t *cur_prot, vm_prot_t *max_prot, vm_inherit_t inherit);

#if __arm64e__
#define xpacd(pointer) do { \
    asm("xpacd %[value]" : [value] "+r" (pointer)); \
} while(false)
#define xpaci(pointer) do { \
    asm("xpaci %[value]" : [value] "+r" (pointer)); \
} while(false)
#define paciza(pointer) do { \
    asm("paciza %[value]" : [value] "+r" (pointer)); \
} while(false)
#define pacia(pointer, context) do { \
    asm("mov x9, #" #context); \
    asm("pacia %[value], x9" : [value] "+r" (pointer)); \
} while(false)
#define pacda(pointer, context) do { \
    asm("mov x9, #" #context); \
    asm("pacda %[value], x9" : [value] "+r" (pointer)); \
} while(false)
#define auth_lr(pointer) do { \
    xpaci(pointer); \
    pacia(pointer, 0x77d3); \
} while(false)
#define auth_pc(pointer) do { \
    xpaci(pointer); \
    pacia(pointer, 0x7481); \
} while(false)
#define auth_sp(pointer) do { \
    xpacd(pointer); \
    pacda(pointer, 0xcbed); \
} while(false)
#define __pc __opaque_pc
#define __lr __opaque_lr
#define __sp __opaque_sp
#else
#define xpacd(pointer) do { } while(false)
#define xpaci(pointer) do { } while(false)
#define paciza(pointer) do { } while(false)
#define pacia(pointer, context) do { } while(false)
#define pacda(pointer, context) do { } while(false)
#define auth_lr(pointer) do { } while(false)
#define auth_pc(pointer) do { } while(false)
#define auth_sp(pointer) do { } while(false)
#endif

#define OUT_LABEL(test, label, format, ...) do { \
    if ((test)) break; \
    int saved_errno = errno; \
    fprintf(stderr, "Injector(%d): " format "\n", __LINE__, ##__VA_ARGS__); \
    errno = saved_errno; \
    goto label; \
} while(false)
#define OUT(test, format, ...) OUT_LABEL(test, out, format, ##__VA_ARGS__)
#define _assert(test) OUT(test, "Assertion failure")
#define _krncall(expr) do { \
    kern_return_t _krnret = (expr); \
    if (_krnret == KERN_SUCCESS) break; \
    OUT(false, "%s", mach_error_string(_krnret)); \
} while(false)

static inline uint64_t SubCallInProcess(mach_port_t task_port, uint64_t function, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6, uint64_t arg7) {
    uint64_t ret = 0;
    const mach_vm_size_t remote_stack_size = 4 * 1024 * 1024;
    mach_vm_address_t remote_stack_base = 0;
    mach_vm_address_t remote_stack_middle = 0;
    mach_vm_address_t return_value = 0;
    mach_vm_size_t return_value_size = sizeof(uint64_t);
    uint64_t magic_value = 0x1337133741414141;
    uint64_t pthread_exit_ptr = 0;
    mach_vm_size_t shellcode_size = 0;
    uint32_t *shellcode = NULL;
    size_t n_shellcode = 0;
    mach_vm_address_t remote_shellcode = 0;
    _STRUCT_ARM_THREAD_STATE64 thread_state = {{0}};
    mach_msg_type_number_t thread_stateCnt = 0;
    mach_vm_address_t loop_lr = 0;
    mach_vm_size_t loop_lr_size = 0;
    mach_vm_address_t remote_pthread = 0;
    mach_vm_size_t remote_pthread_size = 0;
    mach_port_t thread_port = MACH_PORT_NULL;
    uint64_t lr_magic = 0xd63f0260;
    _krncall(mach_vm_allocate(task_port, &remote_stack_base, remote_stack_size, VM_FLAGS_ANYWHERE));
    remote_stack_middle = remote_stack_base + (remote_stack_size / 2);
    _krncall(mach_vm_allocate(task_port, &return_value, return_value_size, VM_FLAGS_ANYWHERE));
    _krncall(mach_vm_write(task_port, return_value, (vm_offset_t)&magic_value, (mach_msg_type_number_t)return_value_size));
    pthread_exit_ptr = (uint64_t)pthread_exit;
    _assert(pthread_exit_ptr != 0);
    xpaci(pthread_exit_ptr);
    xpaci(function);
#define PUSH(x) do { \
    shellcode_size += sizeof(*shellcode); \
    if (shellcode == NULL) { \
        shellcode = malloc(shellcode_size); \
        _assert(shellcode != NULL); \
    } else { \
        shellcode = reallocf(shellcode, shellcode_size); \
        _assert(shellcode != NULL); \
    } \
    shellcode[n_shellcode] = x; \
    n_shellcode++; \
} while(false)
    // Set up arguments
    PUSH(0x58000200); /* ldr x0, #0x40 */
    PUSH(0x58000221); /* ldr x1, #0x44 */
    PUSH(0x58000242); /* ldr x2, #0x48 */
    PUSH(0x58000263); /* ldr x3, #0x4c */
    PUSH(0x58000284); /* ldr x4, #0x50 */
    PUSH(0x580002a5); /* ldr x5, #0x54 */
    PUSH(0x580002c6); /* ldr x6, #0x58 */
    PUSH(0x580002e7); /* ldr x7, #0x5c */
    // Call function
    PUSH(0x58000308); /* ldr x8, #0x60 ; x8 = *(pc + 0x60) */
    PUSH(0xd63f0100); /* blr x8        ; x8(x0, x1, x2, x3, x4, x5, x6, x7) */
    PUSH(0x10000301); /* adr x1, #0x60 ; x1 = pc + 0x60 */
    PUSH(0xf9400021); /* ldr x1, [x1]  ; x1 = *x1 */
    PUSH(0xf9000020); /* str x0, [x1]  ; *x1 = x0 */
    // Call pthread_exit
    PUSH(0xd2800000); /* mov x0, #0x0  ; first argument in x0 */
    PUSH(0x580002c8); /* ldr x8, #0x58 ; x8 = *(pc + 0x58) */
    PUSH(0xd63f0100); /* blr x8        ; x8(x0) */
    // Data
#define PUSH_DATA(x) do { \
    PUSH(x & 0xffffffff); /* Lower x */ \
    PUSH((x & 0xffffffff00000000) >> 32); /* Upper x */ \
} while(false)
    PUSH_DATA(arg0);
    PUSH_DATA(arg1);
    PUSH_DATA(arg2);
    PUSH_DATA(arg3);
    PUSH_DATA(arg4);
    PUSH_DATA(arg5);
    PUSH_DATA(arg6);
    PUSH_DATA(arg7);
    PUSH_DATA(function);
    PUSH_DATA(return_value);
    PUSH_DATA(pthread_exit_ptr);
#undef PUSH_ARG
#undef PUSH
    _krncall(mach_vm_allocate(task_port, &remote_shellcode, shellcode_size, VM_FLAGS_ANYWHERE));
    _krncall(mach_vm_write(task_port, remote_shellcode, (vm_offset_t)shellcode, (mach_msg_type_number_t)shellcode_size));
    _krncall(mach_vm_protect(task_port, remote_shellcode, shellcode_size, false, VM_PROT_READ | VM_PROT_EXECUTE));
    thread_stateCnt = sizeof(thread_state) / 4;
    arm_thread_state64_set_sp(thread_state, (void *)remote_stack_middle);
    loop_lr_size = sizeof(uint32_t);
    _krncall(mach_vm_allocate(task_port, &loop_lr, loop_lr_size, VM_FLAGS_ANYWHERE));
    _krncall(mach_vm_write(task_port, loop_lr, (vm_offset_t)&lr_magic, (mach_msg_type_number_t)loop_lr_size));
    _krncall(mach_vm_protect(task_port, loop_lr, loop_lr_size, false, VM_PROT_READ | VM_PROT_EXECUTE));
    thread_state.__x[19] = loop_lr;
    paciza(loop_lr);
    arm_thread_state64_set_lr_fptr(thread_state, (void *)loop_lr);
    xpaci(loop_lr);
    remote_pthread_size = sizeof(uint64_t);
    _krncall(mach_vm_allocate(task_port, &remote_pthread, remote_pthread_size, VM_FLAGS_ANYWHERE));
    arm_thread_state64_set_pc_fptr(thread_state, (void *)pthread_create_from_mach_thread);
    thread_state.__x[0] = remote_pthread;
    thread_state.__x[1] = 0;
    thread_state.__x[2] = remote_shellcode;
    paciza(thread_state.__x[2]);
    thread_state.__x[3] = 0;
    _krncall(thread_create_running(task_port, ARM_THREAD_STATE64, (thread_state_t)&thread_state, thread_stateCnt, &thread_port));
    while (true) {
        _krncall(thread_get_state(thread_port, ARM_THREAD_STATE64, (thread_state_t)&thread_state, &thread_stateCnt));
        xpaci(thread_state.__pc);
        if ((uint64_t)thread_state.__pc != loop_lr) continue;
        if ((uint64_t)thread_state.__x[19] != loop_lr) continue;
        break;
    }
    while (true) {
        uint64_t remote_ret = 0;
        mach_vm_size_t outsize = 0;
        _krncall(mach_vm_read_overwrite(task_port, return_value, return_value_size, (mach_vm_address_t)&remote_ret, &outsize));
        _assert(outsize == return_value_size);
        if (remote_ret == magic_value) continue;
        ret = remote_ret;
        break;
    }
out:;
    if (shellcode != NULL) {
        free(shellcode);
        shellcode = NULL;
    }
    if (thread_port != MACH_PORT_NULL) {
        thread_terminate(thread_port);
        mach_port_deallocate(mach_task_self(), thread_port);
        thread_port = MACH_PORT_NULL;
    }
    if (remote_stack_base != 0) {
        mach_vm_deallocate(task_port, remote_stack_base, remote_stack_size);
        remote_stack_base = 0;
    }
    if (remote_pthread != 0) {
        mach_vm_deallocate(task_port, remote_pthread, remote_pthread_size);
        remote_pthread = 0;
    }
    if (return_value != 0) {
        mach_vm_deallocate(task_port, return_value, return_value_size);
        return_value = 0;
    }
    if (remote_shellcode != 0) {
        mach_vm_deallocate(task_port, remote_shellcode, shellcode_size);
        remote_shellcode = 0;
    }
    if (loop_lr != 0) {
        mach_vm_deallocate(task_port, loop_lr, loop_lr_size);
        loop_lr = 0;
    }
    return ret;
}

bool SubHookProcess(pid_t pid, const char *library) {
    bool ret = false;
    mach_port_t task_port = MACH_PORT_NULL;
    mach_vm_address_t address = 0;
    mach_vm_size_t address_size = 0;
    if (library[0] != '/') {
        fprintf(stderr, "SubError: require absolute path to %s\n", library);
        goto out;
    }
    _krncall(task_for_pid(mach_task_self(), pid, &task_port));
    _assert(MACH_PORT_VALID(task_port));
    address_size = strlen(library) + 1;
    _krncall(mach_vm_allocate(task_port, &address, address_size, VM_FLAGS_ANYWHERE));
    _krncall(mach_vm_write(task_port, address, (vm_offset_t)library, (mach_msg_type_number_t)address_size));
    ret = (SubCallInProcess(task_port, (uint64_t)dlopen, address, RTLD_LAZY, 0, 0, 0, 0, 0, 0) != 0);
out:;
    if (address != 0) {
        mach_vm_deallocate(task_port, address, address_size);
        address = 0;
    }
    if (task_port != MACH_PORT_NULL) {
        mach_port_deallocate(mach_task_self(), task_port);
        task_port = MACH_PORT_NULL;
    }
    return ret;
}

bool MSHookProcess(pid_t pid, const char *library) {
    return SubHookProcess(pid, library);
}
