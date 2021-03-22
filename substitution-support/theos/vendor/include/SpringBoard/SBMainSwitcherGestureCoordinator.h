@interface SBMainSwitcherGestureCoordinator : NSObject

+ (instancetype)sharedInstance;

- (BOOL)isAppSwitcherShowing;

- (void)_releaseOrientationLock;
- (void)_lockOrientation;

@end
