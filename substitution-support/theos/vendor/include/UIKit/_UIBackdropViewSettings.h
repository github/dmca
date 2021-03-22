@interface _UIBackdropViewSettings : NSObject

- (instancetype)initWithDefaultValues;

@property (nonatomic, retain) UIColor *colorTint;

@property CGFloat colorTintAlpha;
@property CGFloat grayscaleTintLevel;
@property CGFloat grayscaleTintAlpha;

@end
