@interface UIStatusBarAnimationParameters : NSObject

+ (void)animateWithParameters:(UIStatusBarAnimationParameters *)parameters animations:(void (^)(void))animations completion:(void (^)(BOOL finished))completion;

- (instancetype)initWithDefaultParameters;

@end
