#import <Foundation/Foundation.h>

// Event Data Sources represent the metadata associated with specific events
// A data source is constructed automatically for each event in /Library/Activator/Events

@protocol LAEventDataSource <NSObject>

@required
- (NSString *)localizedTitleForEventName:(NSString *)eventName;
- (NSString *)localizedGroupForEventName:(NSString *)eventName;
- (NSString *)localizedDescriptionForEventName:(NSString *)eventName;

@optional
- (BOOL)eventWithNameIsHidden:(NSString *)eventName;
- (BOOL)eventWithNameRequiresAssignment:(NSString *)eventName;
- (BOOL)eventWithName:(NSString *)eventName isCompatibleWithMode:(NSString *)eventMode;
- (BOOL)eventWithNameSupportsUnlockingDeviceToSend:(NSString *)eventName;

- (BOOL)eventWithNameSupportsRemoval:(NSString *)eventName;
- (void)removeEventWithName:(NSString *)eventName;
- (NSString *)configurationViewControllerClassNameForEventWithName:(NSString *)eventName bundle:(NSBundle **)bundle; // LAEventConfigurationViewController
- (id)configurationForEventWithName:(NSString *)eventName;
- (void)eventWithName:(NSString *)eventName didSaveNewConfiguration:(id)configuration;

@end

