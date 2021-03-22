@class SBSApplicationShortcutIcon;

@interface SBSApplicationShortcutItem : NSObject

@property (nonatomic, copy) NSString *type;
@property (nonatomic, copy) NSString *localizedTitle;
@property (nonatomic, copy) NSString *localizedSubtitle;
@property (nonatomic, copy) SBSApplicationShortcutIcon *icon;
@property (nonatomic, copy) NSString *bundleIdentifierToLaunch;

+ (instancetype)staticShortcutItemWithDictionary:(NSDictionary *)dictionary localizationHandler:(/*^block*/id)handler;

@end
