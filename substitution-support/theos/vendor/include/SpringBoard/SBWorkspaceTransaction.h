@class SBWorkspaceTransitionRequest, FBSDisplay;

@interface SBWorkspaceTransaction : NSObject

@property (nonatomic, readonly) FBSDisplay *display;
@property (nonatomic, readonly) SBWorkspaceTransitionRequest *transitionRequest;

- (instancetype)initWithTransitionRequest:(SBWorkspaceTransitionRequest *)request;

- (void)begin;

@end
