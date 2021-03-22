@class SBWorkspaceApplicationTransitionContext, SBWorkspace, FBSDisplay;

@interface SBWorkspaceTransitionRequest : NSObject

@property (nonatomic, readonly) BOOL isMainWorkspaceTransitionRequest;
@property (nonatomic, copy, readonly) NSSet *activatingApps;
@property (nonatomic, copy, readonly) NSSet *deactivatingApps;
@property (nonatomic, readonly) SBWorkspace *workspace;
@property (nonatomic, readonly) FBSDisplay *display;
@property (nonatomic, retain) SBWorkspaceApplicationTransitionContext *applicationContext;

- (instancetype)initWithWorkspace:(SBWorkspace *)workspace display:(FBSDisplay *)display;

@end
