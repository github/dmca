#import "SBWorkspaceTransitionContext.h"

typedef NS_ENUM(NSInteger, SBLayoutRole) {
	SBLayoutRoleUnknown,
	SBLayoutRoleMainApp,
	SBLayoutRoleSideApp
};

@class SBWorkspaceApplication, SBWorkspaceEntity;

@interface SBWorkspaceApplicationTransitionContext : SBWorkspaceTransitionContext

- (SBWorkspaceApplication *)previousApplicationForLayoutRole:(SBLayoutRole)layoutRole;

- (void)setEntity:(SBWorkspaceEntity *)entity forLayoutRole:(SBLayoutRole)role;

@end
