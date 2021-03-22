#ifndef PATCHFINDER64_H_
#define PATCHFINDER64_H_

#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

extern bool auth_ptrs;
extern bool monolithic_kernel;

int init_kernel(size_t (*kread)(uint64_t, void *, size_t), uint64_t kernel_base, const char *filename);
void term_kernel(void);

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

uint64_t find_register_value(uint64_t where, int reg);
uint64_t find_reference(uint64_t to, int n, enum text_bases base);
uint64_t find_strref(const char *string, int n, enum string_bases string_base, bool full_match, bool ppl_base);
uint64_t find_gPhysBase(void);
uint64_t find_kernel_pmap(void);
uint64_t find_amfiret(void);
uint64_t find_ret_0(void);
uint64_t find_amfi_memcmpstub(void);
uint64_t find_sbops(void);
uint64_t find_lwvm_mapio_patch(void);
uint64_t find_lwvm_mapio_newj(void);

uint64_t find_entry(void);
const unsigned char *find_mh(void);

uint64_t find_cpacr_write(void);
uint64_t find_str(const char *string);
uint64_t find_amfiops(void);
uint64_t find_sysbootnonce(void);
uint64_t find_trustcache(void);
uint64_t find_amficache(void);
uint64_t find_allproc(void);
uint64_t find_vfs_context_current(void);
uint64_t find_vnode_lookup(void);
uint64_t find_vnode_put(void);
uint64_t find_vnode_getfromfd(void);
uint64_t find_vnode_getattr(void);
uint64_t find_SHA1Init(void);
uint64_t find_SHA1Update(void);
uint64_t find_SHA1Final(void);
uint64_t find_csblob_entitlements_dictionary_set(void);
uint64_t find_kernel_task(void);
uint64_t find_kernproc(void);
uint64_t find_vnode_recycle(void);
uint64_t find_lck_mtx_lock(void);
uint64_t find_lck_mtx_unlock(void);
uint64_t find_strlen(void);
uint64_t find_add_x0_x0_0x40_ret(void);
uint64_t find_boottime(void);
uint64_t find_zone_map_ref(void);
uint64_t find_OSBoolean_True(void);
uint64_t find_osunserializexml(void);
uint64_t find_smalloc(void);
uint64_t find_shenanigans(void);
uint64_t find_move_snapshot_to_purgatory(void);
uint64_t find_chgproccnt(void);
uint64_t find_kauth_cred_ref(void);
uint64_t find_apfs_jhash_getvnode(void);
uint64_t find_fs_lookup_snapshot_metadata_by_name_and_return_name(void);
uint64_t find_fs_lookup_snapshot_metadata_by_name(void);
uint64_t find_mount_common(void);
uint64_t find_fs_snapshot(void);
uint64_t find_vnode_get_snapshot(void);
uint64_t find_pmap_load_trust_cache(void);
uint64_t find_paciza_pointer__l2tp_domain_module_start(void);
uint64_t find_paciza_pointer__l2tp_domain_module_stop(void);
uint64_t find_l2tp_domain_inited(void);
uint64_t find_sysctl__net_ppp_l2tp(void);
uint64_t find_sysctl_unregister_oid(void);
uint64_t find_mov_x0_x4__br_x5(void);
uint64_t find_mov_x9_x0__br_x1(void);
uint64_t find_mov_x10_x3__br_x6(void);
uint64_t find_kernel_forge_pacia_gadget(void);
uint64_t find_kernel_forge_pacda_gadget(void);
uint64_t find_IOUserClient__vtable(void);
uint64_t find_IORegistryEntry__getRegistryEntryID(void);
uint64_t find_cs_blob_generation_count(void);
uint64_t find_cs_find_md(void);
uint64_t find_cs_validate_csblob(void);
uint64_t find_kalloc_canblock(void);
uint64_t find_ubc_cs_blob_allocate_site(void);
uint64_t find_kfree(void);
uint64_t find_hook_cred_label_update_execve(void);
uint64_t find_flow_divert_connect_out(void);
uint64_t find_pmap_loaded_trust_caches(void);
uint64_t find_unix_syscall(void);
uint64_t find_pthread_kext_register(void);
uint64_t find_pthread_callbacks(void);
uint64_t find_unix_syscall_return(void);
uint64_t find_sysent(void);
uint64_t find_syscall(int n);
uint64_t find_proc_find(void);
uint64_t find_proc_rele(void);
// EX: find_mpo(cred_label_update_execve)
#define find_mpo(name) find_mpo_entry(offsetof(struct mac_policy_ops, mpo_ ##name))
uint64_t find_mpo_entry(uint64_t offset);
uint64_t find_hook_policy_syscall(int n);
uint64_t find_hook_mount_check_snapshot_revert();
uint64_t find_syscall_set_profile(void);
uint64_t find_syscall_check_sandbox(void);
uint64_t find_sandbox_set_container_copyin(void);
uint64_t find_platform_set_container(void);
uint64_t find_extension_create_file(void);
uint64_t find_extension_add(void);
uint64_t find_extension_release(void);
uint64_t find_sfree(void);
uint64_t find_sb_ustate_create(void);
uint64_t find_sstrdup(void);
uint64_t find_handler_map(void);
uint64_t find_sandbox_handler(const char *name);
uint64_t find_issue_extension_for_mach_service(void);
uint64_t find_issue_extension_for_absolute_path(void);
uint64_t find_copy_path_for_vp(void);
uint64_t find_vn_getpath(void);
uint64_t find_IOMalloc(void);
uint64_t find_IOFree(void);

uint64_t find_symbol(const char *symbol);

#endif
