@class FBSDisplay, SBMainWorkspace, SBWorkspaceTransaction;

@interface SBWorkspace : NSObject

+ (SBMainWorkspace *)mainWorkspace;

@property (nonatomic, readonly) FBSDisplay *display;
@property (nonatomic, readonly) SBWorkspaceTransaction *currentTransaction;

@end
