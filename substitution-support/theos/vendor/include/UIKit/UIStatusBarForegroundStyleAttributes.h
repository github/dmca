#import "UIStatusBarItemView.h"

@interface UIStatusBarForegroundStyleAttributes : NSObject

- (instancetype)initWithHeight:(CGFloat)height legibilityStyle:(int)legibilityStyle tintColor:(UIColor *)tintColor hasBusyBackground:(BOOL)hasBusyBackground NS_AVAILABLE_IOS(8_3);
- (instancetype)initWithHeight:(CGFloat)height legibilityStyle:(int)legibilityStyle tintColor:(UIColor *)tintColor hasBusyBackground:(BOOL)hasBusyBackground idiom:(UIUserInterfaceIdiom)idiom NS_AVAILABLE_IOS(8_3);


@property (nonatomic) CGFloat height;

- (UIFont *)textFontForStyle:(UIStatusBarItemViewTextStyle)style;

- (NSString *)expandedNameForImageName:(NSString *)imageName;
- (UIImage *)untintedImageNamed:(NSString *)name;

@property (nonatomic, retain) UIColor *tintColor;

@end
