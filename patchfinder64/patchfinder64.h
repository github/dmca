#ifndef PATCHFINDER64_H_
#define PATCHFINDER64_H_

#ifndef _pf64_external
#define _pf64_external
#endif

#ifndef _pf64_internal
#define _pf64_internal
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

extern bool auth_ptrs;
extern bool monolithic_kernel;
extern uint32_t cmdline_offset;

typedef size_t (*kread_t)(uint64_t, void *, size_t);
_pf64_external int init_kernel(size_t (*kread)(uint64_t, void *, size_t), uint64_t kernel_base, const char *filename);
_pf64_external void term_kernel(void);

enum text_bases {
    text_xnucore_base = 0,
    text_prelink_base,
    text_ppl_base
};

enum string_bases {
    string_base_cstring = 0,
    string_base_pstring,
    string_base_oslstring,
    string_base_data,
    string_base_const
};

_pf64_external uint64_t find_register_value(uint64_t where, int reg);
_pf64_external uint64_t find_reference(uint64_t to, int n, enum text_bases base);
_pf64_external uint64_t find_strref(const char *string, int n, enum string_bases string_base, bool full_match, bool ppl_base);
_pf64_external uint64_t find_str(const char *string, int n, enum string_bases string_base, bool full_match, bool ppl_base);
_pf64_external uint64_t find_gPhysBase(void);
_pf64_external uint64_t find_kernel_pmap(void);
_pf64_external uint64_t find_amfiret(void);
_pf64_external uint64_t find_ret_0(void);
_pf64_external uint64_t find_amfi_memcmpstub(void);
_pf64_external uint64_t find_sbops(void);
_pf64_external uint64_t find_lwvm_mapio_patch(void);
_pf64_external uint64_t find_lwvm_mapio_newj(void);

_pf64_external uint64_t find_entry(void);
_pf64_external const unsigned char *find_mh(void);

_pf64_external uint64_t find_cpacr_write(void);
_pf64_external uint64_t find_amfiops(void);
_pf64_external uint64_t find_sysbootnonce(void);
_pf64_external uint64_t find_trustcache(void);
_pf64_external uint64_t find_amficache(void);
_pf64_external uint64_t find_allproc(void);
_pf64_external uint64_t find_vfs_context_current(void);
_pf64_external uint64_t find_vnode_lookup(void);
_pf64_external uint64_t find_vnode_put(void);
_pf64_external uint64_t find_vnode_getfromfd(void);
_pf64_external uint64_t find_vnode_getattr(void);
_pf64_external uint64_t find_SHA1Init(void);
_pf64_external uint64_t find_SHA1Update(void);
_pf64_external uint64_t find_SHA1Final(void);
_pf64_external uint64_t find_csblob_entitlements_dictionary_set(void);
_pf64_external uint64_t find_kernel_task(void);
_pf64_external uint64_t find_kernproc(void);
_pf64_external uint64_t find_vnode_recycle(void);
_pf64_external uint64_t find_lck_mtx_lock(void);
_pf64_external uint64_t find_lck_mtx_unlock(void);
_pf64_external uint64_t find_strlen(void);
_pf64_external uint64_t find_add_x0_x0_0x40_ret(void);
_pf64_external uint64_t find_boottime(void);
_pf64_external uint64_t find_zone_map(void);
_pf64_external uint64_t find_OSBoolean_True(void);
_pf64_external uint64_t find_osunserializexml(void);
_pf64_external uint64_t find_smalloc(void);
_pf64_external uint64_t find_shenanigans(void);
_pf64_external uint64_t find_boot_args(void);
_pf64_external uint64_t find_move_snapshot_to_purgatory(void);
_pf64_external uint64_t find_chgproccnt(void);
_pf64_external uint64_t find_kauth_cred_ref(void);
_pf64_external uint64_t find_apfs_jhash_getvnode(void);
_pf64_external uint64_t find_fs_lookup_snapshot_metadata_by_name_and_return_name(void);
_pf64_external uint64_t find_fs_lookup_snapshot_metadata_by_name(void);
_pf64_external uint64_t find_mount_common(void);
_pf64_external uint64_t find_fs_snapshot(void);
_pf64_external uint64_t find_vnode_get_snapshot(void);
_pf64_external uint64_t find_pmap_load_trust_cache(void);
_pf64_external uint64_t find_paciza_pointer__l2tp_domain_module_start(void);
_pf64_external uint64_t find_paciza_pointer__l2tp_domain_module_stop(void);
_pf64_external uint64_t find_l2tp_domain_inited(void);
_pf64_external uint64_t find_sysctl__net_ppp_l2tp(void);
_pf64_external uint64_t find_sysctl_unregister_oid(void);
_pf64_external uint64_t find_mov_x0_x4__br_x5(void);
_pf64_external uint64_t find_mov_x9_x0__br_x1(void);
_pf64_external uint64_t find_mov_x10_x3__br_x6(void);
_pf64_external uint64_t find_kernel_forge_pacia_gadget(void);
_pf64_external uint64_t find_kernel_forge_pacda_gadget(void);
_pf64_external uint64_t find_IOUserClient__vtable(void);
_pf64_external uint64_t find_IORegistryEntry__getRegistryEntryID(void);
_pf64_external uint64_t find_cs_blob_generation_count(void);
_pf64_external uint64_t find_cs_find_md(void);
_pf64_external uint64_t find_cs_validate_csblob(void);
_pf64_external uint64_t find_kalloc_canblock(void);
_pf64_external uint64_t find_ubc_cs_blob_allocate_site(void);
_pf64_external uint64_t find_kfree(void);
_pf64_external uint64_t find_hook_cred_label_update_execve(void);
_pf64_external uint64_t find_flow_divert_connect_out(void);
_pf64_external uint64_t find_pmap_loaded_trust_caches(void);
_pf64_external uint64_t find_unix_syscall(void);
_pf64_external uint64_t find_pthread_kext_register(void);
_pf64_external uint64_t find_pthread_callbacks(void);
_pf64_external uint64_t find_unix_syscall_return(void);
_pf64_external uint64_t find_sysent(void);
_pf64_external uint64_t find_syscall(int n);
_pf64_external uint64_t find_proc_find(void);
_pf64_external uint64_t find_proc_rele(void);
// EX: find_mpo(cred_label_update_execve)
#define find_mpo(name) find_mpo_entry(offsetof(struct mac_policy_ops, mpo_ ##name))
_pf64_external uint64_t find_mpo_entry(uint64_t offset);
_pf64_external uint64_t find_hook_vnode_check_exec(void);
_pf64_external uint64_t find_hook_policy_syscall(int n);
_pf64_external uint64_t find_hook_mount_check_snapshot_revert(void);
_pf64_external uint64_t find_syscall_set_profile(void);
_pf64_external uint64_t find_syscall_check_sandbox(void);
_pf64_external uint64_t find_sandbox_set_container_copyin(void);
_pf64_external uint64_t find_platform_set_container(void);
_pf64_external uint64_t find_extension_create_file(void);
_pf64_external uint64_t find_extension_add(void);
_pf64_external uint64_t find_extension_release(void);
_pf64_external uint64_t find_sfree(void);
_pf64_external uint64_t find_sb_ustate_create(void);
_pf64_external uint64_t find_sstrdup(void);
_pf64_external uint64_t find_handler_map(void);
_pf64_external uint64_t find_sandbox_handler(const char *name);
_pf64_external uint64_t find_issue_extension_for_mach_service(void);
_pf64_external uint64_t find_issue_extension_for_absolute_path(void);
_pf64_external uint64_t find_copy_path_for_vp(void);
_pf64_external uint64_t find_vn_getpath(void);
_pf64_external uint64_t find_IOMalloc(void);
_pf64_external uint64_t find_IOFree(void);
_pf64_external uint64_t find_ppl_stubs(void);
_pf64_external uint64_t find_ppl_stub(uint16_t id);
_pf64_external uint64_t find_pmap_cs_cd_register(void);
_pf64_external uint64_t find_pmap_cs_cd_unregister(void);
_pf64_external uint64_t find_check_for_signature(void);
_pf64_external uint64_t find_sysctl__vm_cs_blob_size_max(void);
_pf64_external uint64_t find_ubc_cs_blob_add_site(void);
_pf64_external uint64_t find_ubc_cs_blob_add(void);
_pf64_external uint64_t find_kernel_memory_allocate(void);
_pf64_external uint64_t find_kernel_map(void);
_pf64_external uint64_t find_ipc_space_kernel(void);
_pf64_external uint64_t find_copyin(void);
_pf64_external uint64_t find_copyout(void);
_pf64_external uint64_t find_socketops(void);
_pf64_external uint64_t find_realhost(void);
_pf64_external uint64_t find_convert_port_to_task(void) ;
_pf64_external size_t size_sbops(void);
_pf64_external const char **list_sbops(void);
_pf64_external size_t offsetof_sbop(const char *sbop);

_pf64_external uint64_t find_symbol(const char *symbol);

#endif
