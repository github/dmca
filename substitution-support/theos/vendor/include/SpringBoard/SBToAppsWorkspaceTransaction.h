#import "SBMainWorkspaceTransaction.h"

@interface SBToAppsWorkspaceTransaction : SBMainWorkspaceTransaction

@property (nonatomic, readonly) NSArray *activatingApplications;
@property (nonatomic, readonly) NSArray *deactivatingApplications;
@property (nonatomic, readonly) NSArray *toApplications;
@property (nonatomic, readonly) NSArray *fromApplications;
@property (nonatomic, readonly) NSSet *trackedProcesses;

- (BOOL)isFromMainSwitcher;
- (BOOL)isFromSideSwitcher;
- (BOOL)isGoingToMainSwitcher;

@end
