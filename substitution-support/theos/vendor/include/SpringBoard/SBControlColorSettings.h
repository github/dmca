@interface SBControlColorSettings : NSObject

+ (instancetype)settingsWithTintColor:(UIColor *)tintColor selectedTintColor:(UIColor *)selectedTextColor textColor:(UIColor *)textColor selectedTextColor:(UIColor *)selectedTextColor;

- (instancetype)initWithTintColor:(UIColor *)tintColor selectedTintColor:(UIColor *)selectedTextColor textColor:(UIColor *)textColor selectedTextColor:(UIColor *)selectedTextColor;

@property (nonatomic, retain, readonly) UIColor *selectedTextColor;
@property (nonatomic, retain, readonly) UIColor *textColor;
@property (nonatomic, retain, readonly) UIColor *selectedTintColor;
@property (nonatomic, retain, readonly) UIColor *tintColor;

@end
