@interface SBControlCenterController : NSObject

+ (instancetype)sharedInstance;

- (void)presentAnimated:(BOOL)animated completion:(void(^)())completion;
- (void)dismissAnimated:(BOOL)animated completion:(void(^)())completion;

@property (nonatomic, assign, getter=isUILocked) BOOL UILocked;

- (BOOL)_shouldShowGrabberOnFirstSwipe;

@end
