#import <UIKit/UIKit.h>

// Settings View Controllers represent view controllers that can be displayed the Activator app, Activator's settings pane, and possibly in other Activator-integrated apps

#ifndef LA_SETTINGS_CONTROLLER
#define LA_SETTINGS_CONTROLLER(superclass) : superclass
#endif

@interface LASettingsViewController LA_SETTINGS_CONTROLLER(UIViewController)
+ (id)controller;
- (id)init;
@end

@interface LARootSettingsController LA_SETTINGS_CONTROLLER(LASettingsViewController)
@end

@interface LAModeSettingsController LA_SETTINGS_CONTROLLER(LASettingsViewController)
- (id)initWithMode:(NSString *)mode;
@end

@interface LAEventSettingsController LA_SETTINGS_CONTROLLER(LASettingsViewController)
- (id)initWithModes:(NSArray *)modes eventName:(NSString *)eventName;
@end

@interface LAListenerSettingsViewController LA_SETTINGS_CONTROLLER(LASettingsViewController)
@property (nonatomic, copy) NSString *listenerName;
@end

@interface LAEventConfigurationViewController LA_SETTINGS_CONTROLLER(LASettingsViewController)
- (id)initWithEventName:(NSString *)eventName;
@property (nonatomic, readonly, copy) NSString *eventName;
@property (nonatomic, assign) BOOL showsSaveButton;
- (BOOL)performSave;
@end

@interface LAListenerConfigurationViewController LA_SETTINGS_CONTROLLER(LASettingsViewController)
- (id)initWithListenerName:(NSString *)listenerName;
@property (nonatomic, readonly, copy) NSString *listenerName;
@property (nonatomic, assign) BOOL showsSaveButton;
- (BOOL)performSave;
@end
