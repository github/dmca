@class FBScene, FBSceneHostWrapperView;

@interface FBSceneHostManager : NSObject

@property (nonatomic, retain, readonly) FBScene *scene;

- (FBSceneHostWrapperView *)hostViewForRequester:(NSString *)requester enableAndOrderFront:(BOOL)orderFront;

- (void)enableHostingForRequester:(NSString *)requester orderFront:(BOOL)front;
- (void)disableHostingForRequester:(NSString *)requester;

@end
