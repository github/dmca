//
//  Utility.hpp
//  Azad
//
//  Created by Umang Raghuvanshi on 02/08/19.
//  Copyright © 2019 Umang Raghuvanshi. All rights reserved.
//

#ifndef Utility_hpp
#define Utility_hpp

#include <vector>
extern "C" {
#include <assert.h>
#include <iokit.h>
#include <mach/mach.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
}

#ifdef IOSURFACE_EXTERN
#define extern IOSURFACE_EXTERN
#endif

namespace Azad {
class KernelExploit;
class Parameters;
namespace Utility {
    struct ool_msg {
        mach_msg_header_t hdr;
        mach_msg_body_t body;
        mach_msg_ool_ports_descriptor_t ool_ports;
    };

    mach_port_t sprayPortPointer(mach_port_t targetPortName, unsigned int count,
        int disposition);
    uint64_t findPort(const mach_port_t targetPort, const uint64_t currentTaskKaddr,
        const Parameters& params, KernelExploit& exploit);
} // namespace Utility
} // namespace Azad

extern "C" {
kern_return_t mach_vm_allocate(vm_map_t target, mach_vm_address_t* address,
    mach_vm_size_t size, int flags);
kern_return_t mach_vm_read_overwrite(vm_map_t target_task,
    mach_vm_address_t address,
    mach_vm_size_t size,
    mach_vm_address_t data,
    mach_vm_size_t* outsize);
kern_return_t mach_vm_write(vm_map_t target_task, mach_vm_address_t address,
    vm_offset_t data, mach_msg_type_number_t dataCnt);
kern_return_t mach_vm_deallocate(vm_map_t target, mach_vm_address_t address,
    mach_vm_size_t size);
;
kern_return_t mach_vm_read(vm_map_t target_task, mach_vm_address_t address,
    mach_vm_size_t size, vm_offset_t* data,
    mach_msg_type_number_t* dataCnt);
}

#endif /* Utility_hpp */
