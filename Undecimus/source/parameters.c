/*
 * parameters.c
 * Brandon Azad
 */
#define PARAMETERS_EXTERN
#include "parameters.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>

#include "log.h"
#include "platform.h"
#include <common.h>

// ---- General system parameters -----------------------------------------------------------------

// Initialization for general system parameters.
static void
init__system_parameters()
{
    STATIC_ADDRESS(kernel_base) = 0xFFFFFFF007004000;
    if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_2) {
        kernel_slide_step = 0x1000;
    } else {
        kernel_slide_step = 0x200000;
    }
    message_size_for_kmsg_zone = 76;
    kmsg_zone_size = 256;
    max_ool_ports_per_message = 16382;
    gc_step = 2 * MB;
}

// ---- Offset initialization ---------------------------------------------------------------------

// Initialization for iOS 13.0.

static void
init__offsets_13_0()
{
	OFFSET(host, special) = 0x10;

	SIZE(ipc_entry)              = 0x18;
	OFFSET(ipc_entry, ie_object) =  0x0;
	OFFSET(ipc_entry, ie_bits)   =  0x8;

	OFFSET(ipc_port, ip_bits)       =  0x0;
	OFFSET(ipc_port, ip_references) =  0x4;
	OFFSET(ipc_port, ip_receiver)   = 0x60;
	OFFSET(ipc_port, ip_kobject)    = 0x68;
	OFFSET(ipc_port, ip_mscount)    = 0x9c;
	OFFSET(ipc_port, ip_srights)    = 0xa0;

	OFFSET(ipc_space, is_table_size) = 0x14;
	OFFSET(ipc_space, is_table)      = 0x20;
	OFFSET(ipc_space, is_task)       = 0x28;

	OFFSET(proc, p_list_next) =  0x0;
	OFFSET(proc, task)        = 0x10;
	OFFSET(proc, p_pid)       = 0x68;

	OFFSET(task, lck_mtx_data)        =   0x0;
	OFFSET(task, lck_mtx_type)        =   0xb;
	OFFSET(task, ref_count)           =  0x10;
	OFFSET(task, active)              =  0x14;
	OFFSET(task, map)                 =  0x28;
	OFFSET(task, itk_sself)           = 0x108;
	OFFSET(task, itk_space)           = 0x320;
	OFFSET(task, bsd_info)            = 0x388;
	OFFSET(task, all_image_info_addr) = 0x3d0;

	OFFSET(IOSurface, properties) = 0xe8;

	OFFSET(IOSurfaceClient, surface) = 0x40;

	OFFSET(IOSurfaceRootUserClient, surfaceClients) = 0x118;

	OFFSET(OSArray, count) = 0x14;
	OFFSET(OSArray, array) = 0x20;

	OFFSET(OSData, capacity) = 0x10;
	OFFSET(OSData, data) = 0x18;

	OFFSET(OSDictionary, count) = 0x14;
	OFFSET(OSDictionary, dictionary) = 0x20;

	OFFSET(OSString, string) = 0x10;
}

// Initialization for iOS 12.0.
static void
init__offsets_12_0()
{
    OFFSET(filedesc, fd_ofiles) = 0;

    OFFSET(fileglob, fg_ops) = 0x28;
    OFFSET(fileglob, fg_data) = 0x38;
    
    OFFSET(fileops, fo_type) = 0;

    OFFSET(fileproc, f_fglob) = 8;

    SIZE(ipc_entry) = 0x18;
    OFFSET(ipc_entry, ie_object) = 0;
    OFFSET(ipc_entry, ie_bits) = 8;
    OFFSET(ipc_entry, ie_request) = 16;

    SIZE(ipc_port) = 0xa8;
    BLOCK_SIZE(ipc_port) = 0x4000;
    OFFSET(ipc_port, ip_bits) = 0;
    OFFSET(ipc_port, ip_references) = 4;
    OFFSET(ipc_port, waitq_flags) = 24;
    OFFSET(ipc_port, imq_messages) = 64;
    OFFSET(ipc_port, imq_msgcount) = 80;
    OFFSET(ipc_port, imq_qlimit) = 82;
    OFFSET(ipc_port, ip_receiver) = 96;
    OFFSET(ipc_port, ip_kobject) = 104;
    OFFSET(ipc_port, ip_nsrequest) = 112;
    OFFSET(ipc_port, ip_requests) = 128;
    OFFSET(ipc_port, ip_mscount) = 156;
    OFFSET(ipc_port, ip_srights) = 160;

    SIZE(ipc_port_request) = 0x10;
    OFFSET(ipc_port_request, ipr_soright) = 0;

    OFFSET(ipc_space, is_table_size) = 0x14;
    OFFSET(ipc_space, is_table) = 0x20;
    OFFSET(ipc_space, is_task) = 0x28;

    SIZE(ipc_voucher) = 0x50;
    BLOCK_SIZE(ipc_voucher) = 0x4000;

    OFFSET(pipe, pipe_buffer) = 0x10;

    OFFSET(proc, p_pid) = 0x60;
    OFFSET(proc, p_ucred) = 0xf8;
    OFFSET(proc, p_fd) = 0x100;
    OFFSET(proc, task) = 0x10;

    SIZE(ip6_pktopts) = 192;
    OFFSET(ip6_pktopts, ip6po_pktinfo) = 16;
    OFFSET(ip6_pktopts, ip6po_minmtu) = 180;

    SIZE(sysctl_oid) = 0x50;
    OFFSET(sysctl_oid, oid_parent) = 0x0;
    OFFSET(sysctl_oid, oid_link) = 0x8;
    OFFSET(sysctl_oid, oid_kind) = 0x14;
    OFFSET(sysctl_oid, oid_handler) = 0x30;
    OFFSET(sysctl_oid, oid_version) = 0x48;
    OFFSET(sysctl_oid, oid_refcnt) = 0x4c;

    OFFSET(task, lck_mtx_type) = 0xb;
    OFFSET(task, ref_count) = 0x10;
    OFFSET(task, active) = 0x14;
    OFFSET(task, map) = 0x20;
    OFFSET(task, itk_space) = 0x300;
#if __arm64e__
    OFFSET(task, bsd_info) = 0x368;
#else
    OFFSET(task, bsd_info) = 0x358;
#endif
    OFFSET(task, itk_registered) = 0x2e8;
}

// Initialization for iOS 11.0.
static void
init__offsets_11_0()
{
    OFFSET(filedesc, fd_ofiles) = 0;

    OFFSET(fileglob, fg_ops) = 0x28;
    OFFSET(fileglob, fg_data) = 0x38;
    
    OFFSET(fileops, fo_type) = 0;

    OFFSET(fileproc, f_fglob) = 8;

    SIZE(ipc_entry) = 0x18;
    OFFSET(ipc_entry, ie_object) = 0;
    OFFSET(ipc_entry, ie_bits) = 8;
    OFFSET(ipc_entry, ie_request) = 16;

    SIZE(ipc_port) = 0xa8;
    BLOCK_SIZE(ipc_port) = 0x4000;
    OFFSET(ipc_port, ip_bits) = 0;
    OFFSET(ipc_port, ip_references) = 4;
    OFFSET(ipc_port, waitq_flags) = 24;
    OFFSET(ipc_port, imq_messages) = 0x40;
    OFFSET(ipc_port, imq_msgcount) = 0x50;
    OFFSET(ipc_port, imq_qlimit) = 0x52;
    OFFSET(ipc_port, ip_receiver) = 0x60;
    OFFSET(ipc_port, ip_kobject) = 0x68;
    OFFSET(ipc_port, ip_nsrequest) = 0x70;
    OFFSET(ipc_port, ip_requests) = 0x80;
    OFFSET(ipc_port, ip_mscount) = 0x9c;
    OFFSET(ipc_port, ip_srights) = 0xa0;

    SIZE(ipc_port_request) = 0x10;
    OFFSET(ipc_port_request, ipr_soright) = 0;

    OFFSET(ipc_space, is_table_size) = 0x14;
    OFFSET(ipc_space, is_table) = 0x20;
    OFFSET(ipc_space, is_task) = 0x28;

    SIZE(ipc_voucher) = 0x50;
    BLOCK_SIZE(ipc_voucher) = 0x4000;

    OFFSET(pipe, pipe_buffer) = 0x10;

    OFFSET(proc, p_pid) = 0x10;
    OFFSET(proc, p_ucred) = 0x100;
    OFFSET(proc, p_fd) = 0x108;
    OFFSET(proc, task) = 0x18;

    SIZE(ip6_pktopts) = 192;
    OFFSET(ip6_pktopts, ip6po_pktinfo) = 16;
    OFFSET(ip6_pktopts, ip6po_minmtu) = 180;

    SIZE(sysctl_oid) = 0x50;
    OFFSET(sysctl_oid, oid_parent) = 0x0;
    OFFSET(sysctl_oid, oid_link) = 0x8;
    OFFSET(sysctl_oid, oid_kind) = 0x14;
    OFFSET(sysctl_oid, oid_handler) = 0x30;
    OFFSET(sysctl_oid, oid_version) = 0x48;
    OFFSET(sysctl_oid, oid_refcnt) = 0x4c;

    OFFSET(task, lck_mtx_type) = 0xb;
    OFFSET(task, ref_count) = 0x10;
    OFFSET(task, active) = 0x14;
    OFFSET(task, map) = 0x20;
    OFFSET(task, itk_space) = 0x308;
    OFFSET(task, bsd_info) = 0x368;
    OFFSET(task, itk_registered) = 0x2f0;
}

// Initialize offset parameters whose values are computed from other parameters.
static void
init__computed_offsets_12_0()
{
    COUNT_PER_BLOCK(ipc_port) = BLOCK_SIZE(ipc_port) / SIZE(ipc_port);
    COUNT_PER_BLOCK(ipc_voucher) = BLOCK_SIZE(ipc_voucher) / SIZE(ipc_voucher);
}

// ---- Public API --------------------------------------------------------------------------------

bool parameters_init()
{
    // Get general platform info.
    platform_init();
    // Initialize general system parameters.
    init__system_parameters();
    // Initialize offsets.
	if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_13_0) {
		init__offsets_13_0();
	} else if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_0) {
        init__offsets_12_0();
        init__computed_offsets_12_0();
    } else if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_11_0) {
        init__offsets_11_0();
        init__computed_offsets_12_0();
    } else {
        ERROR("no offsets for %f", kCFCoreFoundationVersionNumber);
        return false;
    }
    return true;
}
