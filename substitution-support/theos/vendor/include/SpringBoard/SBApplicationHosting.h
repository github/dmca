@class SBApplication;

@protocol SBApplicationHosting <NSObject>
@required

- (SBApplication *)hostedApp;
- (BOOL)isHostingAnApp;
- (BOOL)canHostAnApp;
- (void)hostedAppWillRotateToInterfaceOrientation:(UIInterfaceOrientation)orientation;

@end