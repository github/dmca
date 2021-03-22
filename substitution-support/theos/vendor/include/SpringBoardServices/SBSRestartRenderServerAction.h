#import "SBSRelaunchAction.h"

@interface SBSRestartRenderServerAction : SBSRelaunchAction

+ (instancetype)restartActionWithTargetRelaunchURL:(NSURL *)url;

@end
