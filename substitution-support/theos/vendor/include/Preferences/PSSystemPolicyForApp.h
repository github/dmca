typedef NS_OPTIONS(NSUInteger, PSSystemPolicyOptions) {
	PSSystemPolicyOptionsNotifications = 1 << 0,
	PSSystemPolicyOptionsContacts = 1 << 2,
	PSSystemPolicyOptionsCalendars = 1 << 3,
	PSSystemPolicyOptionsReminders = 1 << 4,
	PSSystemPolicyOptionsPhotos = 1 << 5
};

@interface PSSystemPolicyForApp : NSObject

- (instancetype)initWithBundleIdentifier:(NSString *)bundleIdentifier;

- (NSArray <PSSpecifier *> *)specifiersForPolicyOptions:(PSSystemPolicyOptions)options force:(BOOL)force;

- (PSSpecifier *)notificationSpecifier;

@end
