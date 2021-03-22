@class FBApplicationProcess, FBProcess;

@interface FBProcessManager : NSObject

+ (instancetype)sharedInstance;

- (FBProcess *)processForPID:(int)pid;

- (FBApplicationProcess *)applicationProcessForPID:(int)pid;
- (FBApplicationProcess *)applicationProcessesForBundleIdentifier:(NSString *)identifier;

- (FBApplicationProcess *)createApplicationProcessForBundleID:(NSString *)identifier;

- (NSArray *)allApplicationProcesses;
- (NSArray *)allProcesses;

@end
