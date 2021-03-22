#import <UIKit/UIKit.h>

#import "rocketbootstrap.h"

@implementation NSObject (rocketbootstrap)

+ (kern_return_t)rocketbootstrap_unlock:(NSString *)name
{
	return rocketbootstrap_unlock([name UTF8String]);
}

static CFDataRef messagePortCallback(CFMessagePortRef local, SInt32 msgid, CFDataRef data, void *info)
{
	NSLog(@"rockettest_messageport_server: received %@", data);
	return CFDataCreate(kCFAllocatorDefault, (const UInt8 *)"bootstrap", 9);
}

+ (kern_return_t)rockettest_messageport_server
{
	static CFMessagePortRef messagePort;
	if (messagePort)
		return 0;
	messagePort = CFMessagePortCreateLocal(kCFAllocatorDefault, CFSTR("rockettest_messageport"), messagePortCallback, NULL, NULL);
	CFRunLoopSourceRef source = CFMessagePortCreateRunLoopSource(kCFAllocatorDefault, messagePort, 0);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), source, kCFRunLoopCommonModes);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), source, (CFStringRef)UITrackingRunLoopMode);
	return rocketbootstrap_cfmessageportexposelocal(messagePort);
}

+ (NSData *)rockettest_messageport_client
{
	CFMessagePortRef remote = rocketbootstrap_cfmessageportcreateremote(kCFAllocatorDefault, CFSTR("rockettest_messageport"));
	if (!remote)
		return nil;
	CFDataRef request = CFDataCreate(kCFAllocatorDefault, (const UInt8 *)"rocket", 6);
	CFDataRef response = NULL;
	CFMessagePortSendRequest(remote, 0, request, 10, 10, CFSTR("rocketboostrap_wait"), &response);
	CFRelease(remote);
	CFRelease(request);
	return [(NSData *)response autorelease];
}

@end
