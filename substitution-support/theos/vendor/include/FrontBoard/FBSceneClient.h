@class FBScene, FBSSceneSettings, FBSSceneSettingsDiff, FBSSceneTransitionContext;

@protocol FBSceneClient <NSObject>

@required

- (void)host:(FBScene *)host didUpdateSettings:(FBSSceneSettings *)settings withDiff:(FBSSceneSettingsDiff *)diff transitionContext:(FBSSceneTransitionContext *)transitionContext completion:(id)completionBlock;

@end
