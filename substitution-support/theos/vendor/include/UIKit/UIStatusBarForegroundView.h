@class UIStatusBarForegroundStyleAttributes;

@interface UIStatusBarForegroundView : UIView

- (instancetype)initWithFrame:(CGRect)frame foregroundStyle:(UIStatusBarForegroundStyleAttributes *)foregroundStyle usesVerticalLayout:(BOOL)usesVerticalLayout; // 7.1 –
- (instancetype)initWithFrame:(CGRect)frame foregroundStyle:(UIStatusBarForegroundStyleAttributes *)foregroundStyle; // 7.0

@property (nonatomic, retain) UIStatusBarForegroundStyleAttributes *foregroundStyle;

@end
