@class SBWallpaperView;

@interface SBUIController : NSObject <UIGestureRecognizerDelegate>

+ (instancetype)sharedInstance;

- (BOOL)clickedMenuButton;
- (BOOL)handleMenuDoubleTap;

- (BOOL)handleHomeButtonSinglePressUp;
- (BOOL)handleHomeButtonDoublePressDown;

@property (nonatomic, retain) SBWallpaperView *wallpaperView;

- (UIWindow *)window;

@end
