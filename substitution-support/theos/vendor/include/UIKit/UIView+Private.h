@interface UIView (Private)

- (UIViewController *)_viewControllerForAncestor;

- (void)layoutBelowIfNeeded;

@end
