@interface SBWorkspaceTransitionContext : NSObject

@property (nonatomic, copy, readonly) NSSet *entities;
@property (nonatomic, assign) BOOL animationDisabled; 

@end
