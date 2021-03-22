/*
 *  inject.m
 *  
 *  Created by Sam Bingner on 9/27/2018
 *  Copyright 2018 Sam Bingner. All Rights Reserved.
 *
 */

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#include <mach/mach.h>
#include <dlfcn.h>
#include <offsetcache.h>
#include <patchfinder64.h>
#include "CSCommon.h"
#include "kern_funcs.h"
#include "inject.h"
#include "kernel_call.h"
#include "parameters.h"
#include "kc_parameters.h"
#include "kernel_memory.h"

mach_port_t try_restore_port() {
    mach_port_t port = MACH_PORT_NULL;
    kern_return_t err;

    err = host_get_special_port(mach_host_self(), 0, 4, &port);
    if (err == KERN_SUCCESS && port != MACH_PORT_NULL) {
        // make sure rk64 etc use this port
        return port;
    }
    fprintf(stderr, "unable to retrieve persisted port\n");
    return MACH_PORT_NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr,"Usage: inject /full/path/to/executable\n");
        fprintf(stderr,"Inject executables to trust cache\n");
        return -1;
    }
    mach_port_t tfp0 = try_restore_port();
    if (tfp0 == MACH_PORT_NULL) {
        fprintf(stderr, "Unable to obtain tfp0\n");
        return -2;
    }
    set_tfp0(tfp0);
    struct task_dyld_info dyld_info = { 0 };
    mach_msg_type_number_t count = TASK_DYLD_INFO_COUNT;
    if (task_info(tfp0, TASK_DYLD_INFO, (task_info_t)&dyld_info, &count) == 0 &&
            dyld_info.all_image_info_addr != 0 &&
            dyld_info.all_image_info_addr != dyld_info.all_image_info_size + 0xfffffff007004000) {

        size_t blob_size = rk64(dyld_info.all_image_info_addr);
        struct cache_blob *blob = create_cache_blob(blob_size);
        if (kread(dyld_info.all_image_info_addr, blob, blob_size)) import_cache_blob(blob);
        free(blob);
        if (get_offset("kernel_slide") == kernel_slide) {
#ifdef DEBUG
            print_cache();
#endif
            if (get_offset("kernel_base")) {
                kernel_base = get_offset("kernel_base");
            } else {
                kernel_base = dyld_info.all_image_info_size + 0xfffffff007004000;
            }
        }
    } else if ((kernel_base = dyld_info.all_image_info_addr) != 0) {
        kernel_slide = dyld_info.all_image_info_size;
        @autoreleasepool {
            NSMutableDictionary *offsets = [NSMutableDictionary dictionaryWithContentsOfFile:@"/jb/offsets.plist"];
            SETOFFSET(trustcache, (uint64_t)strtoull([offsets[@"TrustChain"] UTF8String], NULL, 16));
#if __arm64e__
            SETOFFSET(kernel_task, (uint64_t)strtoull([offsets[@"KernelTask"] UTF8String], NULL, 16));
            // We should use this on other things but kexecute is broken for i6 at least
            SETOFFSET(pmap_load_trust_cache, (uint64_t)strtoull([offsets[@"PmapLoadTrustCache"] UTF8String], NULL, 16));
            SETOFFSET(paciza_pointer__l2tp_domain_module_start, (uint64_t)strtoull([offsets[@"PacizaPointerL2TPDomainModuleStart"] UTF8String], NULL, 16));
            SETOFFSET(paciza_pointer__l2tp_domain_module_stop, (uint64_t)strtoull([offsets[@"PacizaPointerL2TPDomainModuleStop"] UTF8String], NULL, 16));
            SETOFFSET(l2tp_domain_inited, (uint64_t)strtoull([offsets[@"L2TPDomainInited"] UTF8String], NULL, 16));
            SETOFFSET(sysctl__net_ppp_l2tp, (uint64_t)strtoull([offsets[@"SysctlNetPPPL2TP"] UTF8String], NULL, 16));
            SETOFFSET(sysctl_unregister_oid, (uint64_t)strtoull([offsets[@"SysctlUnregisterOid"] UTF8String], NULL, 16));
            SETOFFSET(mov_x0_x4__br_x5, (uint64_t)strtoull([offsets[@"MovX0X4BrX5"] UTF8String], NULL, 16));
            SETOFFSET(mov_x9_x0__br_x1, (uint64_t)strtoull([offsets[@"MovX9X0BrX1"] UTF8String], NULL, 16));
            SETOFFSET(mov_x10_x3__br_x6, (uint64_t)strtoull([offsets[@"MovX10X3BrX6"] UTF8String], NULL, 16));
            SETOFFSET(kernel_forge_pacia_gadget, (uint64_t)strtoull([offsets[@"KernelForgePaciaGadget"] UTF8String], NULL, 16));
            SETOFFSET(kernel_forge_pacda_gadget, (uint64_t)strtoull([offsets[@"KernelForgePacdaGadget"] UTF8String], NULL, 16));
            SETOFFSET(IOUserClient__vtable, (uint64_t)strtoull([offsets[@"IOUserClientVtable"] UTF8String], NULL, 16));
            SETOFFSET(IORegistryEntry__getRegistryEntryID, (uint64_t)strtoull([offsets[@"IORegistryEntryGetRegistryEntryID"] UTF8String], NULL, 16));
#endif
        }
    } else {
        return -3;
    }
#if __arm64e__
    if (GETOFFSET(pmap_load_trust_cache)) pmap_load_trust_cache = _pmap_load_trust_cache;
    parameters_init();
    kernel_task_port = tfp0;
    current_task = rk64(task_self_addr() + OFFSET(ipc_port, ip_kobject));
    kernel_task = rk64(GETOFFSET(kernel_task));
    kernel_call_init();
#endif
    printf("Injecting to trust cache...\n");
    NSMutableArray *files = [NSMutableArray new];
    for (int i=1; i<argc; i++) {
        [files addObject:@( argv[i] )];
    }

    int errs = injectTrustCache(files, GETOFFSET(trustcache), pmap_load_trust_cache);
    if (errs < 0) {
        printf("Error %d injecting to trust cache.\n", errs);
    } else {
        printf("Successfully injected [%d/%d] to trust cache.\n", (int)files.count - errs, (int)files.count);
    }
#if __arm64e__
    kernel_call_deinit();
#endif
    return errs;
}
