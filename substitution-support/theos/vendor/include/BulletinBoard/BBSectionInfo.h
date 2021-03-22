#import "BBSectionInfoSettings.h"

typedef NS_ENUM(NSUInteger, BBSectionInfoAlertType) {
	BBSectionInfoAlertTypeNone,
	BBSectionInfoAlertTypeBanner,
	BBSectionInfoAlertTypeAlert
};

@protocol BBDataProvider;

@interface BBSectionInfo : NSObject

+ (instancetype)defaultSectionInfoForType:(NSUInteger)type;
+ (instancetype)defaultSectionInfoForSection:(id <BBDataProvider>)section;

@property (nonatomic, retain) NSString *sectionID;
@property (nonatomic, retain) NSString *subsectionID;
@property (nonatomic, retain) NSString *displayName;

@property (nonatomic) BOOL allowsNotifications;
@property (nonatomic) BOOL showsInLockScreen;
@property (nonatomic) BOOL showsInNotificationCenter;
@property (nonatomic) BOOL showsMessagePreview;
@property (nonatomic) BOOL showsOnExternalDevices;

@property (nonatomic) BBSectionInfoAlertType alertType;
@property NSUInteger notificationCenterLimit;

@property (nonatomic, retain) BBSectionInfoSettings *sectionInfoSettings;

@property (nonatomic) BBSectionInfoPushSettings pushSettings;

@end
