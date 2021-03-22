const UIWindowLevel UIWindowLevelNotificationCenter = 1056.f;
const UIWindowLevel UIWindowLevelStatusBarLockScreen = 1052.f;
const UIWindowLevel UIWindowLevelAlertReal = 1996.f;
const UIWindowLevel UIWindowLevelTextEffects = 2100.f;

@interface UIWindow (Private)

+ (UIWindow *)keyWindow;
+ (NSArray *)allWindowsIncludingInternalWindows:(BOOL)internalWindows onlyVisibleWindows:(BOOL)onlyVisibleWindows;

- (void)_updateToInterfaceOrientation:(UIInterfaceOrientation)orientation duration:(double)duration force:(BOOL)force;
- (void)_setRotatableViewOrientation:(UIInterfaceOrientation)orientation updateStatusBar:(BOOL)updateStatusBar duration:(CGFloat)duration force:(BOOL)force;

@property BOOL keepContextInBackground;
@property (getter=_isSecure, setter=_setSecure:) BOOL _secure;

@property (nonatomic, retain, readonly) UIResponder *firstResponder;

@end
