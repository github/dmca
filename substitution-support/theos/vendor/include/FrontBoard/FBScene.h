#import "FBSceneClient.h"
#import "FBSceneClientProvider.h"

@class FBSSceneSettings, FBSMutableSceneSettings, FBSSceneTransitionContext, FBProcess, FBWindowContextHostManager;

@interface FBScene : NSObject

@property (nonatomic, retain, readonly) FBProcess *clientProcess;

@property (nonatomic, retain, readonly) FBSSceneSettings *settings;

@property (nonatomic, retain, readonly) FBSMutableSceneSettings *mutableSettings;

@property (nonatomic, retain, readonly) id<FBSceneClient> client;

@property (nonatomic, retain, readonly) id<FBSceneClientProvider> clientProvider;

@property (nonatomic, copy, readonly) NSString *identifier;

@property (nonatomic, retain, readonly) FBWindowContextHostManager *contextHostManager;

- (void)_applyMutableSettings:(FBSMutableSceneSettings *)mutableSettings withTransitionContext:(FBSSceneTransitionContext *)transitionContext completion:(id)completionBlock;

@end
