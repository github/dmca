#import "SBApplicationSceneEntity.h"

@class SBApplication;

@interface SBDeviceApplicationSceneEntity : SBApplicationSceneEntity

- (instancetype)initWithApplicationForMainDisplay:(SBApplication *)application;

@end