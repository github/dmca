@interface SBWindowHidingManager : NSObject

+ (instancetype)sharedInstance;

- (void)setAlpha:(CGFloat)alpha forWindow:(UIWindow *)window;

@end