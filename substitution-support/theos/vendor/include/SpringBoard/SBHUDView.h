@interface SBHUDView : UIView

+ (CGFloat)progressIndicatorStep;

- (instancetype)initWithHUDViewLevel:(NSInteger)level;

@property (nonatomic, retain) UIImage *image;
@property CGFloat progress;

@end
