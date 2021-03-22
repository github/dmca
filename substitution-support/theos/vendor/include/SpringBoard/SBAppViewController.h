#import <UIKit/UIViewController.h>
#import "SBApplicationHosting.h"

@class SBApplication, SBAppView, SBWorkspaceApplication. SBApplicationSceneEntity;

@interface SBAppViewController : UIViewController <SBApplicationHosting>
@property (nonatomic, copy, readonly) NSString *bundleIdentifier;
@property (assign, nonatomic) BOOL automatesLifecycle;
@property (assign, nonatomic) NSInteger requestedMode;
@property (nonatomic, readonly) NSInteger currentMode;
@property (nonatomic, readonly) SBAppView *appView;
@property (assign, nonatomic) NSUInteger options;
@property (assign, nonatomic) BOOL ignoresOcclusions;

- (instancetype)initWithIdentifier:(NSString *)identifier andApplication:(SBWorkspaceApplication *)application NS_DEPRECATED_IOS(8_0, 11_0);
- (instancetype)initWithIdentifier:(NSString *)identifier andApplicationSceneEntity:(SBApplicationSceneEntity *)applicationSceneEntity NS_AVAILABLE_IOS(11_0);

- (SBApplication *)hostedApp;
- (BOOL)isHostingAnApp;
- (BOOL)canHostAnApp;
- (void)hostedAppWillRotateToInterfaceOrientation:(UIInterfaceOrientation)orientation;

- (void)invalidate;

@end