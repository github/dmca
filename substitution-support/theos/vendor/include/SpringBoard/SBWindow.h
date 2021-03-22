#import <FrontBoard/FBWindow.h>

@interface SBWindow : FBWindow

- (instancetype)initWithScreen:(UIScreen *)screen debugName:(NSString *)debugName rootViewController:(UIViewController *)viewController;

- (BOOL)isActive;
- (UIInterfaceOrientation)activeInterfaceOrientation;

@end