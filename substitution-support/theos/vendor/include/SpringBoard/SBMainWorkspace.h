#import "SBWorkspace.h"

@class FBSceneManager;

@interface SBMainWorkspace : SBWorkspace

+ (instancetype)sharedInstance;
+ (void)start;

@property (nonatomic, readonly) FBSceneManager *sceneManager;
@property (nonatomic, readonly, getter=isMedusaEnabled) BOOL medusaEnabled;
@property (nonatomic, readonly, getter=isSpringBoardActive) BOOL springBoardActive;
@property (nonatomic, readonly) CGFloat autoDimTime;
@property (nonatomic, readonly) CGFloat autoLockTime;

- (void)handleReachabilityModeActivated;
- (void)handleReachabilityModeDeactivated;

-(void)_suspend;
-(void)_resume;

@end
