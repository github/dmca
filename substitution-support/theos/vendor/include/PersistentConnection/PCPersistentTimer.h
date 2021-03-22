@interface PCPersistentTimer : NSObject

- (instancetype)initWithFireDate:(NSDate *)fireDate serviceIdentifier:(NSString *)serviceIdentifier target:(id)target selector:(SEL)selector userInfo:(id)userInfo;

- (void)scheduleInRunLoop:(NSRunLoop *)runLoop;

@end
