#ifndef kutils_h
#define kutils_h

#include <common.h>
#include <mach/mach.h>
#include <offsetcache.h>
#include <stdbool.h>

#if 0
Credits:
- https://stek29.rocks/2018/01/26/sandbox.html
- https://stek29.rocks/2018/12/11/shenanigans.html
- http://newosxbook.com/QiLin/qilin.pdf
- https://github.com/Siguza/v0rtex/blob/e6d54c97715d6dbcdda8b9a8090484a7a47019d0/src/v0rtex.m#L1623
#endif

#define set_kernel_offset(name, offset) set_offset(#name, offset)
#define kernel_offset(name) get_offset(#name)
#define set_kernel_size(name, size) set_offset(#name, (uint64_t)size)
#define kernel_size(name) (uint32_t)get_offset(#name)
#define set_kernel_offsetof(name, size) set_offset(#name, (uint64_t)size)
#define kernel_offsetof(name) (uint32_t)get_offset(#name)

#define OSBoolTrue getOSBool(true)
#define OSBoolFalse getOSBool(false)
#define _ipc_space_kernel ReadKernel64(task_self_addr() + koffset(KSTRUCT_OFFSET_IPC_PORT_IP_RECEIVER))
#define _kernel_map ReadKernel64(_kernel_task + koffset(KSTRUCT_OFFSET_TASK_VM_MAP))
#define _ubc_cs_blob_allocate_site kernel_offset(ubc_cs_blob_allocate_site)
#define _kernproc ReadKernel64(kernel_offset(kernproc))
#define _kernel_task ReadKernel64(_kernproc + koffset(KSTRUCT_OFFSET_PROC_TASK))
#define _zone_map ReadKernel64(kernel_offset(zone_map_ref))
#define _cs_blob_generation_count ReadKernel32(kernel_offset(cs_blob_generation_count))
#define _allproc ReadKernel64(kernel_offset(allproc))
#define _realhost ReadKernel64(kernel_offset(realhost))

extern kptr_t kernel_base;
extern uint64_t kernel_slide;

extern kptr_t cached_proc_struct_addr;
extern kptr_t cached_task_self_addr;
extern bool found_offsets;
extern bool no_kernel_execution;

#define have_kernel_execution() (!no_kernel_execution)

kptr_t task_self_addr(void);
kptr_t find_kernel_base(void);
kptr_t current_thread(void);
mach_port_t kernel_host_port(void);
bool iterate_proc_list(void (^handler)(kptr_t, pid_t, bool *));
kptr_t get_proc_struct_for_pid(pid_t pid);
kptr_t proc_struct_addr(void);
kptr_t get_address_of_port(kptr_t proc, mach_port_t port);
kptr_t get_kernel_cred_addr(void);
kptr_t give_creds_to_process_at_addr(kptr_t proc, kptr_t cred_addr);
bool set_platform_binary(kptr_t proc, bool set);
kptr_t zm_fix_addr(kptr_t addr);
bool verify_tfp0(void);
extern int (*pmap_load_trust_cache)(kptr_t kernel_trust, size_t length);
int _pmap_load_trust_cache(kptr_t kernel_trust, size_t length);
bool set_host_type(host_t host, uint32_t type);
bool export_tfp0(host_t host);
bool unexport_tfp0(host_t host);
bool set_csflags(kptr_t proc, uint32_t flags, bool value);
bool set_cs_platform_binary(kptr_t proc, bool value);
bool set_csb_platform_binary(kptr_t proc, bool value);
bool set_p_flag(kptr_t proc, uint32_t flag, bool value);
bool set_p_reboot(kptr_t proc, bool value);
bool kernel_set_signals_enabled(pid_t pid, bool enabled);
bool set_child_drained(kptr_t proc, bool value);
bool set_vm_map_flags(kptr_t proc, uint32_t flags, bool value);
bool set_has_corpse_footprint(kptr_t proc, bool value);
bool execute_with_credentials(kptr_t proc, kptr_t credentials, void (^function)(void));
uint32_t get_proc_memstat_state(kptr_t proc);
bool set_proc_memstat_state(kptr_t proc, uint32_t memstat_state);
bool set_proc_memstat_internal(kptr_t proc, bool set);
bool get_proc_memstat_internal(kptr_t proc);
size_t kstrlen(kptr_t ptr);
kptr_t sstrdup(const char *str);
kptr_t smalloc(size_t size);
void sfree(kptr_t ptr);
kptr_t IOMalloc(vm_size_t size);
void IOFree(kptr_t address, vm_size_t size);
int extension_create_file(kptr_t saveto, kptr_t sb, const char *path, size_t path_len, uint32_t subtype);
int extension_create_mach(kptr_t saveto, kptr_t sb, const char *name, uint32_t subtype);
int extension_add(kptr_t ext, kptr_t sb, const char *desc);
void extension_release(kptr_t ext);
void extension_destroy(kptr_t ext);
bool set_file_extension(kptr_t sandbox, const char *exc_key, const char *path);
bool set_mach_extension(kptr_t sandbox, const char *exc_key, const char *name);
kptr_t proc_find(pid_t pid);
void proc_rele(kptr_t proc);
void proc_lock(kptr_t proc);
void proc_unlock(kptr_t proc);
void proc_ucred_lock(kptr_t proc);
void proc_ucred_unlock(kptr_t proc);
void vnode_lock(kptr_t vp);
void vnode_unlock(kptr_t vp);
void mount_lock(kptr_t mp);
void mount_unlock(kptr_t mp);
void task_set_platform_binary(kptr_t task, boolean_t is_platform);
void kauth_cred_ref(kptr_t cred);
void kauth_cred_unref(kptr_t cred);
int chgproccnt(uid_t uid, int diff);
kptr_t vfs_context_current(void);
int vnode_lookup(const char *path, int flags, kptr_t *vpp, kptr_t ctx);
int vnode_getfromfd(kptr_t ctx, int fd, kptr_t *vpp);
int vn_getpath(kptr_t vp, char *pathbuf, int *len);
int vnode_put(kptr_t vp);
bool OSDictionary_SetItem(kptr_t OSDictionary, const char *key, kptr_t val);
kptr_t OSDictionary_GetItem(kptr_t OSDictionary, const char *key);
bool OSDictionary_Merge(kptr_t OSDictionary, kptr_t OSDictionary2);
uint32_t OSDictionary_ItemCount(kptr_t OSDictionary);
kptr_t OSDictionary_ItemBuffer(kptr_t OSDictionary);
kptr_t OSDictionary_ItemKey(kptr_t buffer, uint32_t idx);
kptr_t OSDictionary_ItemValue(kptr_t buffer, uint32_t idx);
uint32_t OSArray_ItemCount(kptr_t OSArray);
bool OSArray_Merge(kptr_t OSArray, kptr_t OSArray2);
kptr_t OSArray_GetObject(kptr_t OSArray, uint32_t idx);
void OSArray_RemoveObject(kptr_t OSArray, uint32_t idx);
kptr_t OSArray_ItemBuffer(kptr_t OSArray);
kptr_t OSObjectFunc(kptr_t OSObject, uint32_t off);
void OSObject_Release(kptr_t OSObject);
void OSObject_Retain(kptr_t OSObject);
uint32_t OSObject_GetRetainCount(kptr_t OSObject);
uint32_t OSString_GetLength(kptr_t OSString);
kptr_t OSString_CStringPtr(kptr_t OSString);
char *OSString_CopyString(kptr_t OSString);
kptr_t OSUnserializeXML(const char *buffer);
kptr_t get_exception_osarray(const char **exceptions, bool is_file_extension);
char **copy_amfi_entitlements(kptr_t present);
kptr_t getOSBool(bool value);
bool entitle_process(kptr_t amfi_entitlements, const char *key, kptr_t val);
bool set_sandbox_exceptions(kptr_t sandbox);
bool check_for_exception(char **current_exceptions, const char *exception);
bool set_amfi_exceptions(kptr_t amfi_entitlements, const char *exc_key, const char **exceptions, bool is_file_extension);
bool set_exceptions(kptr_t sandbox, kptr_t amfi_entitlements);
kptr_t get_amfi_entitlements(kptr_t cr_label);
kptr_t get_sandbox(kptr_t cr_label);
bool entitle_process_with_pid(pid_t pid, const char *key, kptr_t val);
bool remove_memory_limit(void);
bool restore_kernel_task_port(task_t *out_kernel_task_port);
bool restore_kernel_base(uint64_t *out_kernel_base, uint64_t *out_kernel_slide);
bool restore_kernel_offset_cache(void);
bool restore_file_offset_cache(const char *offset_cache_file_path, kptr_t *out_kernel_base, uint64_t *out_kernel_slide);
bool convert_port_to_task_port(mach_port_t port, kptr_t space, kptr_t task_kaddr);
kptr_t make_fake_task(kptr_t vm_map);
bool make_port_fake_task_port(mach_port_t port, kptr_t task_kaddr);
bool remap_tfp0_set_hspn(int slot, task_t port);
kptr_t get_vnode_for_path(const char *path);
kptr_t get_vnode_for_fd(int fd);
char *get_path_for_vnode(kptr_t vnode);
kptr_t get_vnode_for_snapshot(int fd, char *name);
bool set_kernel_task_info(void);
int issue_extension_for_mach_service(kptr_t sb, kptr_t ctx, const char *entry_name, void *desc);
bool unrestrict_process(pid_t pid);
bool unrestrict_process_with_task_port(task_t task_port);
bool revalidate_process(pid_t pid);
bool revalidate_process_with_task_port(task_t task_port);
bool enable_mapping_for_library(pid_t pid, int fd);
kptr_t find_vnode_with_fd(kptr_t proc, int fd);
kptr_t find_vnode_with_path(const char *path);
kptr_t swap_sandbox_for_proc(kptr_t proc, kptr_t sandbox);
int cs_validate_csblob(kptr_t address, size_t length, kptr_t *rcd, kptr_t *rentitlements);
void kfree(kptr_t address, vm_size_t size);
kptr_t cs_find_md(uint8_t type);
kptr_t kalloc_canblock(vm_size_t *size, boolean_t canblock, kptr_t site);
kern_return_t kernel_memory_allocate(kptr_t map, vm_offset_t *addrp, vm_size_t size, vm_offset_t mask, int flags, uint16_t tag);
bool dump_offset_cache(const char *file);
bool restore_offset_cache(const char *file);
bool find_kernel_offsets(void (^unrestrict)(void), void (^rerestrict)(void));
bool kernel_os_dict_lookup(kptr_t os_dict, const char *key, kptr_t *val);
bool kernel_forge_pmap_cs_entries_internal(kptr_t csblobs);
bool kernel_forge_pmap_cs_entries(const char *path);
bool kernel_add_signatures(const char *path);
bool kernel_forge_csblobs(const char *path);
bool set_csb_entitlements(void);
bool set_csb_entitlements_blob(void);
bool kernel_hook_file(const char *original_file, const char *hook_file, const char *old_file);
bool kernel_resign_physical_object(const char *file, const char *name, const char *dylib);
bool kernel_set_generator(const char *generator);

#endif /* kutils_h */
