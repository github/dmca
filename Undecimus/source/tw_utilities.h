//
//  exploit_utilities.h
//  sock_port
//
//  Created by Jake James on 7/17/19.
//  Copyright © 2019 Jake James. All rights reserved.
//

#ifndef exploit_utilities_h
#define exploit_utilities_h

#import <stdio.h>
#import <unistd.h>
#import <stdlib.h>
#import <errno.h>
#import <mach/mach.h>
#import <sched.h>
#include "IOKitLib.h"
#import <sys/utsname.h>

#import "IOSurface_stuff.h"

struct ool_msg  {
    mach_msg_header_t hdr;
    mach_msg_body_t body;
    mach_msg_ool_ports_descriptor_t ool_ports;
};

struct simple_msg {
    mach_msg_header_t hdr;
    char buf[0];
};

typedef struct {
    mach_msg_bits_t       msgh_bits;
    mach_msg_size_t       msgh_size;
    uint64_t              msgh_remote_port;
    uint64_t              msgh_local_port;
    mach_port_name_t      msgh_voucher_port;
    mach_msg_id_t         msgh_id;
} kern_mach_msg_header_t;

struct ool_kmsg  {
    kern_mach_msg_header_t hdr;
    mach_msg_body_t body;
    mach_msg_ool_ports_descriptor_t ool_ports;
};

struct simple_kmsg {
    kern_mach_msg_header_t hdr;
    char buf[0];
};

mach_port_t new_mach_port(void);
kern_return_t send_message(mach_port_t destination, void *buffer, mach_msg_size_t size);
struct simple_msg* receive_message(mach_port_t source, mach_msg_size_t size);
int send_ool_ports(mach_port_t where, mach_port_t target_port, int count, int disposition);

void trigger_gc(void);

#endif /* exploit_utilities_h */
