@interface SBBulletinWindowController : NSObject

+ (instancetype)sharedInstance;

- (void)setBusy:(BOOL)busy forReason:(NSString *)reason;

@end