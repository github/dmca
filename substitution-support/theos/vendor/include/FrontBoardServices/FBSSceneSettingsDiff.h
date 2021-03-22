@class FBSSceneSettings;

@interface FBSSceneSettingsDiff : NSObject

+ (instancetype)diffFromSettings:(FBSSceneSettings *)oldSettings toSettings:(FBSSceneSettings *)newSettings;

@end
