/*
 * mach_vm.h
 * Brandon Azad
 */
#ifndef OOB_TIMESTAMP__MACH_VM__H_
#define OOB_TIMESTAMP__MACH_VM__H_

#include <mach/mach.h>

extern
kern_return_t mach_vm_allocate
(
	vm_map_t target,
	mach_vm_address_t *address,
	mach_vm_size_t size,
	int flags
);

extern
kern_return_t mach_vm_deallocate
(
	vm_map_t target,
	mach_vm_address_t address,
	mach_vm_size_t size
);

extern
kern_return_t mach_vm_protect
(
	vm_map_t target_task,
	mach_vm_address_t address,
	mach_vm_size_t size,
	boolean_t set_maximum,
	vm_prot_t new_protection
);

extern
kern_return_t mach_vm_write
(
	vm_map_t target_task,
	mach_vm_address_t address,
	vm_offset_t data,
	mach_msg_type_number_t dataCnt
);

extern
kern_return_t mach_vm_read_overwrite
(
	vm_map_t target_task,
	mach_vm_address_t address,
	mach_vm_size_t size,
	mach_vm_address_t data,
	mach_vm_size_t *outsize
);

extern
kern_return_t mach_vm_remap
(
	vm_map_t target_task,
	mach_vm_address_t *target_address,
	mach_vm_size_t size,
	mach_vm_offset_t mask,
	int flags,
	vm_map_t src_task,
	mach_vm_address_t src_address,
	boolean_t copy,
	vm_prot_t *cur_protection,
	vm_prot_t *max_protection,
	vm_inherit_t inheritance
);

extern
kern_return_t mach_vm_region_recurse
(
	vm_map_t target_task,
	mach_vm_address_t *address,
	mach_vm_size_t *size,
	natural_t *nesting_depth,
	vm_region_recurse_info_t info,
	mach_msg_type_number_t *infoCnt
);

#endif
