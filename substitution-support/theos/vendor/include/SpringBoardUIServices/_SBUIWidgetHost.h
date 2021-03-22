@protocol _SBUIWidgetHost

- (void)invalidatePreferredViewSize;
- (void)requestLaunchOfURL:(NSURL *)url;
- (void)requestPresentationOfViewController:(UIViewController *)viewController presentationStyle:(UIModalPresentationStyle)style context:(void *)context completion:(void (^)(void))completion;

@end
