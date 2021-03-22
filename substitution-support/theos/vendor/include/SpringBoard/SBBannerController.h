@interface SBBannerController : NSObject

+ (instancetype)sharedInstance;

- (void)dismissBannerWithAnimation:(BOOL)animate reason:(NSInteger)reason forceEvenIfBusy:(BOOL)forceEvenIfBusy;
- (void)_replaceIntervalElapsed;
- (void)_dismissIntervalElapsed;

@end
