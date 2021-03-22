//
//  IOAccelerator_stuff.h
//  time_waste
//
//  Created by Jake James on 2/22/20.
//  Copyright © 2020 Jake James. All rights reserved.
//

#ifndef IOAccelerator_stuff_h
#define IOAccelerator_stuff_h

#import <stdio.h>
#include "IOKitLib.h"
#import <mach/mach.h>
#import <sys/mman.h>

#include "IOSurface_stuff.h"

#define IOAccelCommandQueue2_type 4
#define IOAccelSharedUserClient2_type 2
#define IOAccelSharedUserClient2_create_shmem_selector 5
#define IOAccelSharedUserClient2_destroy_shmem_selector 6
#define IOAccelCommandQueue2_set_notification_port_selector 0
#define IOAccelCommandQueue2_submit_command_buffers_selector 1

struct IOAccelDeviceShmemData {
    void *data;
    uint32_t length;
    uint32_t shmem_id;
};

struct IOAccelCommandQueueSubmitArgs_Header {
    uint32_t field_0;
    uint32_t count;
};

struct IOAccelCommandQueueSubmitArgs_Command {
    uint32_t command_buffer_shmem_id;
    uint32_t segment_list_shmem_id;
    uint64_t notify_1;
    uint64_t notify_2;
};

struct IOAccelSegmentListHeader {
    uint32_t field_0;
    uint32_t field_4;
    uint32_t segment_count;
    uint32_t length;
};

struct IOAccelSegmentResourceList_ResourceGroup {
    uint32_t resource_id[6];
    uint8_t field_18[48];
    uint16_t resource_flags[6];
    uint8_t field_54[2];
    uint16_t resource_count;
};

struct IOAccelSegmentResourceListHeader {
    uint64_t field_0;
    uint32_t kernel_commands_start_offset;
    uint32_t kernel_commands_end_offset;
    int total_resources;
    uint32_t resource_group_count;
    struct IOAccelSegmentResourceList_ResourceGroup resource_groups[];
};

struct IOAccelKernelCommand {
    uint32_t type;
    uint32_t size;
};

struct IOAccelKernelCommand_CollectTimeStamp {
    struct IOAccelKernelCommand command;
    uint64_t timestamp;
};

kern_return_t IOAccelSharedUserClient2_create_shmem(size_t size, struct IOAccelDeviceShmemData *shmem);
kern_return_t IOAccelSharedUserClient2_destroy_shmem(uint32_t shmem_id);
kern_return_t IOAccelCommandQueue2_set_notification_port(mach_port_t notification_port);
kern_return_t IOAccelCommandQueue2_submit_command_buffers(const struct IOAccelCommandQueueSubmitArgs_Header *submit_args, size_t size);

int alloc_shmem(uint32_t buffer_size, struct IOAccelDeviceShmemData *cmdbuf, struct IOAccelDeviceShmemData *seglist);
int overflow_n_bytes(uint32_t buffer_size, int n, struct IOAccelDeviceShmemData *cmdbuf, struct IOAccelDeviceShmemData *seglist);
int make_buffer_readable_by_kernel(void *buffer, uint64_t n_pages);

int init_IOAccelerator(void);
void term_IOAccelerator(void);

extern io_connect_t IOAccelCommandQueue2;
extern io_connect_t IOAccelSharedUserClient2;
extern io_service_t IOGraphicsAccelerator2;

#endif /* IOAccelerator_stuff_h */
