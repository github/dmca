@interface SBAppSwitcherPeopleButtonAndLabelView : UIView

- (instancetype)initWithFrame:(CGRect)frame forMonogramSize:(CGFloat)monogramSize compact:(BOOL)compact;

@property (nonatomic, retain) NSString *title;
@property (nonatomic, retain) UIImage *image;
@property (nonatomic, retain) id legibilitySettings;
@property UIImageRenderingMode imageRenderingMode;

@end
