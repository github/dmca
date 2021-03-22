@class SBApplication, SBAppViewController;

@protocol SBAppViewControllerDelegate <NSObject>
@optional

- (BOOL)appViewController:(SBAppViewController *)controller shouldTransitionToMode:(NSInteger)mode;
- (void)appViewController:(SBAppViewController *)controller didTransitionFromMode:(NSInteger)fromMode toMode:(NSInteger)toMode;
- (void)appViewControllerWillActivateApplication:(SBApplication *)application;
- (void)appViewControllerDidDeactivateApplication:(SBApplication *)application;

@end