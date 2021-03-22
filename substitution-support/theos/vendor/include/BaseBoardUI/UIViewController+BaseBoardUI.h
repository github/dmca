@interface UIViewController (BaseBoardUI)

- (BOOL)bs_addChildViewController:(UIViewController *)childController;
- (BOOL)bs_addChildViewController:(UIViewController *)childController animated:(BOOL)animated transitionBlock:(void(^)())block;

- (BOOL)bs_removeChildViewController:(UIViewController *)childController;
- (BOOL)bs_removeChildViewController:(UIViewController *)childController animated:(BOOL)animated transitionBlock:(void(^)())block;

@end
