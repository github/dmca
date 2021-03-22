#import "UIStatusBarAnimationParameters.h"

typedef NS_ENUM(NSInteger, UIStatusBarHideAnimation) {
	UIStatusBarHideAnimationUnknown0,
	UIStatusBarHideAnimationUnknown1,
	UIStatusBarHideAnimationUnknown2
};

@interface UIStatusBarHideAnimationParameters : UIStatusBarAnimationParameters

@property (nonatomic) UIStatusBarHideAnimation hideAnimation;

@end
