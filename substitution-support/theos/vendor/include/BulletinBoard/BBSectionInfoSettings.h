typedef NS_OPTIONS(NSUInteger, BBSectionInfoPushSettings) {
	BBSectionInfoPushSettingsBadges = 1 << 0,
	BBSectionInfoPushSettingsSounds = 1 << 1,
	BBSectionInfoPushSettingsAlerts = 1 << 2
};

@interface BBSectionInfoSettings : NSObject

@property (nonatomic) BOOL showsInNotificationCenter;
@property (nonatomic) BOOL showsInLockScreen;
@property (nonatomic) BOOL showsOnExternalDevices;

@property (nonatomic) BOOL showsMessagePreview;

@property (nonatomic) BBSectionInfoPushSettings pushSettings;

@end
