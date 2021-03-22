#import "SBActivationSettings.h"

static NSString *const kSBAppTagsHidden = @"hidden";

@class FBScene;

@interface SBApplication : NSObject

@property (nonatomic, retain, readonly) NSString *bundleIdentifier NS_AVAILABLE_IOS(8_0); // Technically available in iOS 5 as well (https://github.com/MP0w/iOS-Headers/blob/master/iOS5.0/SpringBoard/SB.h#L143) and even iOS 4, but you probably don't want to use that (see: Camera/Photos).
@property (nonatomic, retain, readonly) NSString *displayName;
@property (nonatomic, retain, readonly) NSString *displayIdentifier NS_DEPRECATED_IOS(4_0, 8_0);

@property (nonatomic, retain, readonly) NSString *sandboxPath;
@property (nonatomic, retain, readonly) NSString *bundleContainerPath;
@property (nonatomic, retain, readonly) NSString *path;

@property (nonatomic, retain, setter=_setTags:) NSArray *tags;

@property (nonatomic, copy, readonly) NSArray *staticApplicationShortcutItems NS_AVAILABLE_IOS(9_3);
@property (nonatomic, copy) NSArray *staticShortcutItems NS_DEPRECATED_IOS(9_0, 9_3);

@property (readonly, nonatomic) pid_t pid;

- (NSNumber *)badgeNumberOrString;

- (BOOL)isRunning;
- (FBScene *)mainScene;

- (void)clearDeactivationSettings;

- (id)objectForActivationSetting:(SBActivationSetting)activationSetting;
- (NSInteger)flagForActivationSetting:(SBActivationSetting)activationSetting;
- (BOOL)boolForActivationSetting:(SBActivationSetting)activationSetting;
- (void)setObject:(id)object forActivationSetting:(SBActivationSetting)activationSetting;
- (void)setFlag:(NSInteger)flag forActivationSetting:(SBActivationSetting)activationSetting;
- (void)setBool:(BOOL)value forActivationSetting:(SBActivationSetting)activationSetting;

@end
