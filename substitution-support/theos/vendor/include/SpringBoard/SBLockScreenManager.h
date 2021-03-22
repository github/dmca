@class SBLockScreenViewController;

@interface SBLockScreenManager : NSObject

+ (instancetype)sharedInstance;

- (void)lockUIFromSource:(NSUInteger)source withOptions:(NSDictionary *)options;
- (void)unlockUIFromSource:(NSUInteger)source withOptions:(NSDictionary *)options;

@property (nonatomic, retain) SBLockScreenViewController *lockScreenViewController;

@property (readonly) BOOL isUILocked;

@end
