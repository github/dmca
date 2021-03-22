@class UIStatusBarForegroundStyleAttributes, UIStatusBarForegroundView;

typedef NS_ENUM(NSInteger, UIStatusBarPosition) {
	UIStatusBarPositionLeft,
	UIStatusBarPositionRight,
	UIStatusBarPositionCenter
};

@interface UIStatusBar : UIView {
	UIStatusBarForegroundView *_foregroundView;
}

+ (UIStatusBarStyle)defaultStatusBarStyle;
+ (UIStatusBarStyle)defaultStatusBarStyleWithTint:(BOOL)withTint;

+ (CGFloat)heightForStyle:(UIStatusBarStyle)statusBarStyle orientation:(UIInterfaceOrientation)orientation;

+ (UIStatusBarForegroundStyleAttributes *)_styleAttributesForStatusBarStyle:(UIStatusBarStyle)style legacy:(BOOL)legacy;

@property (nonatomic, retain) UIColor *foregroundColor;

@end
