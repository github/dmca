#if TARGET_OS_IPHONE
@interface NSDistributedNotificationCenter : NSNotificationCenter

#ifdef __IPHONE_8_0
@property (class, readonly, strong) NSDistributedNotificationCenter *defaultCenter;
#else
+ (instancetype)defaultCenter;
#endif

@property BOOL suspended;

- (void)postNotificationName:(NSString *)name object:(id)object userInfo:(NSDictionary *)userInfo deliverImmediately:(BOOL)deliverImmediately;

@end
#else
#include_next <Foundation/NSDistributedNotificationCenter.h>
#endif
