#import "rocketbootstrap_internal.h"

#import <CaptainHook/CaptainHook.h>
#import <libkern/OSAtomic.h>
#import <substrate.h>

static OSSpinLock spin_lock;

kern_return_t bootstrap_look_up3(mach_port_t bp, const name_t service_name, mach_port_t *sp, pid_t target_pid, const uuid_t instance_id, uint64_t flags) __attribute__((weak_import));
kern_return_t (*_bootstrap_look_up3)(mach_port_t bp, const name_t service_name, mach_port_t *sp, pid_t target_pid, const uuid_t instance_id, uint64_t flags);

kern_return_t $bootstrap_look_up3(mach_port_t bp, const name_t service_name, mach_port_t *sp, pid_t target_pid, const uuid_t instance_id, uint64_t flags)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSMutableDictionary *threadDictionary = [NSThread currentThread].threadDictionary;
	id obj = [threadDictionary objectForKey:@"rocketbootstrap_intercept_next_lookup"];
	if (obj) {
		[threadDictionary removeObjectForKey:@"rocketbootstrap_intercept_next_lookup"];
		[pool drain];
		return rocketbootstrap_look_up(bp, service_name, sp);
	}
	[pool drain];
	return _bootstrap_look_up3(bp, service_name, sp, target_pid, instance_id, flags);
}

static void hook_bootstrap_lookup(void)
{
	static bool hooked_bootstrap_look_up;
	OSSpinLockLock(&spin_lock);
	if (!hooked_bootstrap_look_up) {
		MSHookFunction(bootstrap_look_up3, $bootstrap_look_up3, (void **)&_bootstrap_look_up3);
		hooked_bootstrap_look_up = true;
	}
	OSSpinLockUnlock(&spin_lock);
}

CFMessagePortRef rocketbootstrap_cfmessageportcreateremote(CFAllocatorRef allocator, CFStringRef name)
{
	if (rocketbootstrap_is_passthrough())
		return CFMessagePortCreateRemote(allocator, name);
	hook_bootstrap_lookup();
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSMutableDictionary *threadDictionary = [NSThread currentThread].threadDictionary;
	[threadDictionary setObject:(id)kCFBooleanTrue forKey:@"rocketbootstrap_intercept_next_lookup"];
	CFMessagePortRef result = CFMessagePortCreateRemote(allocator, name);
	[threadDictionary removeObjectForKey:@"rocketbootstrap_intercept_next_lookup"];
	[pool drain];
	return result;
}

kern_return_t rocketbootstrap_cfmessageportexposelocal(CFMessagePortRef messagePort)
{
	if (rocketbootstrap_is_passthrough())
		return 0;
	CFStringRef name = CFMessagePortGetName(messagePort);
	if (!name)
		return -1;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	kern_return_t result = rocketbootstrap_unlock([(NSString *)name UTF8String]);
	[pool drain];
	return result;
}

@interface CPDistributedMessagingCenter : NSObject
- (void)_setupInvalidationSource;
@end

%group messaging_center

static bool has_hooked_messaging_center;

%hook CPDistributedMessagingCenter

- (mach_port_t)_sendPort
{
	if (objc_getAssociatedObject(self, &has_hooked_messaging_center)) {
		mach_port_t *_sendPort = CHIvarRef(self, _sendPort, mach_port_t);
		NSLock **_lock = CHIvarRef(self, _lock, NSLock *);
		if (_sendPort && _lock) {
			[*_lock lock];
			mach_port_t result = *_sendPort;
			if (result == MACH_PORT_NULL) {
				NSString **_centerName = CHIvarRef(self, _centerName, NSString *);
				if (_centerName && *_centerName && [self respondsToSelector:@selector(_setupInvalidationSource)]) {
					mach_port_t bootstrap = MACH_PORT_NULL;
					task_get_bootstrap_port(mach_task_self(), &bootstrap);
					rocketbootstrap_look_up(bootstrap, [*_centerName UTF8String], _sendPort);
					[self _setupInvalidationSource];
					result = *_sendPort;
				}
			}
			[*_lock unlock];
			return result;
		}
	}
	return %orig();
}

- (void)runServerOnCurrentThreadProtectedByEntitlement:(id)entitlement
{
	if (objc_getAssociatedObject(self, &has_hooked_messaging_center)) {
		NSString **_centerName = CHIvarRef(self, _centerName, NSString *);
		if (_centerName && *_centerName) {
			rocketbootstrap_unlock([*_centerName UTF8String]);
		}
	}
	%orig();
}

%end

%end

void rocketbootstrap_distributedmessagingcenter_apply(CPDistributedMessagingCenter *messaging_center)
{
	if (rocketbootstrap_is_passthrough())
		return;
	OSSpinLockLock(&spin_lock);
	if (!has_hooked_messaging_center) {
		has_hooked_messaging_center = true;
		%init(messaging_center);
	}
	OSSpinLockUnlock(&spin_lock);
	objc_setAssociatedObject(messaging_center, &has_hooked_messaging_center, (id)kCFBooleanTrue, OBJC_ASSOCIATION_ASSIGN);
}
