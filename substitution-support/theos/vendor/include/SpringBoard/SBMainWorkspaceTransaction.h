#import "SBWorkspaceTransaction.h"

@class SBMainWorkspaceTransitionRequest;

@interface SBMainWorkspaceTransaction : SBWorkspaceTransaction

- (instancetype)initWithTransitionRequest:(SBMainWorkspaceTransitionRequest *)request;

@end
