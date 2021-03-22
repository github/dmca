typedef NS_ENUM(NSUInteger, UIStatusBarItemViewTextStyle) {
	UIStatusBarItemViewTextStyleRegular = 1,
	UIStatusBarItemViewTextStyleBold = 3
};

typedef NSUInteger UIStatusBarItemViewActions; // TODO

@class UIStatusBarItem, UIStatusBarForegroundStyleAttributes, UIStatusBarLayoutManager, _UILegibilityImageSet;

@interface UIStatusBarItemView : UIView

+ (instancetype)createViewForItem:(UIStatusBarItem *)item withData:(id)data actions:(UIStatusBarItemViewActions)actions foregroundStyle:(UIStatusBarForegroundStyleAttributes *)style;

- (instancetype)initWithItem:(UIStatusBarItem *)item data:(id)data actions:(UIStatusBarItemViewActions)actions style:(UIStatusBarForegroundStyleAttributes *)style;

- (_UILegibilityImageSet *)imageWithShadowNamed:(NSString *)imageName;
- (_UILegibilityImageSet *)imageWithText:(NSString *)text;

- (_UILegibilityImageSet *)contentsImage;

- (CGFloat)updateContentsAndWidth;

@property (nonatomic, retain) UIStatusBarForegroundStyleAttributes *foregroundStyle;
@property (nonatomic, retain) UIStatusBarLayoutManager *layoutManager;

@property BOOL persistentAnimationsEnabled;

@property (readonly) CGFloat standardPadding;

@end
