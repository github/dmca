#include <sys/cdefs.h>
#include <mach/mach.h>
#include "bootstrap.h"

__BEGIN_DECLS
#ifndef ROCKETBOOTSTRAP_LOAD_DYNAMIC

kern_return_t rocketbootstrap_look_up(mach_port_t bp, const name_t service_name, mach_port_t *sp);


kern_return_t rocketbootstrap_unlock(const name_t service_name); // Errors if not in a privileged process such as SpringBoard or backboardd
kern_return_t rocketbootstrap_register(mach_port_t bp, name_t service_name, mach_port_t sp); // Errors if not in a privileged process such as SpringBoard or backboardd

#ifdef __COREFOUNDATION_CFMESSAGEPORT__
CFMessagePortRef rocketbootstrap_cfmessageportcreateremote(CFAllocatorRef allocator, CFStringRef name);
kern_return_t rocketbootstrap_cfmessageportexposelocal(CFMessagePortRef messagePort);
#endif

#ifdef __OBJC__
@class CPDistributedMessagingCenter;
void rocketbootstrap_distributedmessagingcenter_apply(CPDistributedMessagingCenter *messaging_center);
#endif

#else
#include "rocketbootstrap_dynamic.h"
#endif
__END_DECLS
