@class UIStatusBar;

typedef enum {
	UIBackgroundStyleDefault,
	UIBackgroundStyleTransparent,
	UIBackgroundStyleLightBlur,
	UIBackgroundStyleDarkBlur,
	UIBackgroundStyleDarkTranslucent
} UIBackgroundStyle;

@interface UIApplication (Private)

@property (nonatomic, retain) UIStatusBar *statusBar;

- (void)_setBackgroundStyle:(UIBackgroundStyle)backgroundStyle;

@end
