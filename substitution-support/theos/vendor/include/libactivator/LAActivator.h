#import <UIKit/UIKit.h>
#import <libkern/OSAtomic.h>

#import "LAActivatorVersion.h"

// Main event dispatcher that is responsible for sending events, maintaining the list of actions/events, and querying information about them

@class LAEvent, LAListenerConfigurationViewController, LAEventConfigurationViewController;
@protocol LAListener, LAEventDataSource;

@interface LAActivator : NSObject LA_PRIVATE_IVARS(LAActivator)

+ (LAActivator *)sharedInstance;

@property (nonatomic, readonly) LAActivatorVersion version;
@property (nonatomic, readonly, getter=isRunningInsideSpringBoard) BOOL runningInsideSpringBoard;
@property (nonatomic, readonly, getter=isDangerousToSendEvents) BOOL dangerousToSendEvents;

// Listeners

- (id<LAListener>)listenerForEvent:(LAEvent *)event;
- (void)sendEventToListener:(LAEvent *)event;
- (void)sendEvent:(LAEvent *)event toListenerWithName:(NSString *)listenerName;
- (void)sendEvent:(LAEvent *)event toListenersWithNames:(NSArray *)listenerNames;
- (void)sendAbortToListener:(LAEvent *)event;
- (void)sendAbortEvent:(LAEvent *)event toListenerWithName:(NSString *)listenerName;
- (void)sendAbortEvent:(LAEvent *)event toListenersWithNames:(NSArray *)listenerNames;
- (void)sendPreviewEventToListenerWithName:(NSString *)listenerName;
- (void)sendDeactivateEventToListeners:(LAEvent *)event;

- (id<LAListener>)listenerForName:(NSString *)name;
- (BOOL)hasListenerWithName:(NSString *)name;
- (void)registerListener:(id<LAListener>)listener forName:(NSString *)name; // Only available in SpringBoard
- (void)unregisterListenerWithName:(NSString *)name; // Only available in SpringBoard

- (BOOL)hasSeenListenerWithName:(NSString *)name;

// Assignments

- (void)assignEvent:(LAEvent *)event toListenerWithName:(NSString *)listenerName;
- (void)assignEvent:(LAEvent *)event toListenersWithNames:(NSArray *)listenerNames;
- (void)addListenerAssignment:(NSString *)listenerName toEvent:(LAEvent *)event;
- (void)removeListenerAssignment:(NSString *)listenerName fromEvent:(LAEvent *)event;
- (void)unassignEvent:(LAEvent *)event;
- (NSString *)assignedListenerNameForEvent:(LAEvent *)event;
- (NSArray *)assignedListenerNamesForEvent:(LAEvent *)event;
- (NSArray *)eventsAssignedToListenerWithName:(NSString *)listenerName;

// Events

@property (nonatomic, readonly) NSArray *availableEventNames;
- (BOOL)hasEventWithName:(NSString *)name;
- (BOOL)eventWithNameIsHidden:(NSString *)name;
- (BOOL)eventWithNameRequiresAssignment:(NSString *)name;
- (NSArray *)compatibleModesForEventWithName:(NSString *)name;
- (BOOL)eventWithName:(NSString *)eventName isCompatibleWithMode:(NSString *)eventMode;
- (BOOL)eventWithNameSupportsUnlockingDeviceToSend:(NSString *)eventName;

- (BOOL)eventWithNameSupportsRemoval:(NSString *)eventName;
- (void)removeEventWithName:(NSString *)eventName;

- (void)registerEventDataSource:(id<LAEventDataSource>)dataSource forEventName:(NSString *)eventName;
- (void)unregisterEventDataSourceWithEventName:(NSString *)eventName;

- (BOOL)eventWithNameSupportsConfiguration:(NSString *)eventName;
- (LAEventConfigurationViewController *)configurationViewControllerForEventWithName:(NSString *)eventName;

// Listener Metadata

@property (nonatomic, readonly) NSArray *availableListenerNames;
- (id)infoDictionaryValueOfKey:(NSString *)key forListenerWithName:(NSString *)name;
- (BOOL)listenerWithNameRequiresAssignment:(NSString *)name;
- (NSArray *)compatibleEventModesForListenerWithName:(NSString *)name;
- (BOOL)listenerWithName:(NSString *)listenerName isCompatibleWithMode:(NSString *)eventMode;
- (BOOL)listenerWithName:(NSString *)listenerName isCompatibleWithEventName:(NSString *)eventName;
- (BOOL)listenerWithNameNeedsPoweredDisplay:(NSString *)listenerName;
- (NSArray *)exclusiveAssignmentGroupsForListenerName:(NSString *)listenerName;
- (BOOL)listenerNamesAreMutuallyCompatible:(NSArray *)listenerNames;
- (UIImage *)iconForListenerName:(NSString *)listenerName;
- (UIImage *)smallIconForListenerName:(NSString *)listenerName;
- (UIImage *)imageForListenerName:(NSString *)listenerName usingTemplate:(NSBundle *)templateBundle;
- (BOOL)listenerWithNameSupportsRemoval:(NSString *)listenerName;
- (void)requestRemovalForListenerWithName:(NSString *)listenerName;

- (BOOL)listenerWithNameSupportsConfiguration:(NSString *)listenerName;
- (LAListenerConfigurationViewController *)configurationViewControllerForListenerWithName:(NSString *)listenerName;

// Event Modes

@property (nonatomic, readonly) NSArray *availableEventModes;
@property (nonatomic, readonly) NSString *currentEventMode;
@property (nonatomic, readonly) NSString *currentEventModeUnderneathLockScreen;
@property (nonatomic, readonly) BOOL supportsUnlockingDeviceToSendEvents;

// Blacklisting

@property (nonatomic, readonly) NSString *displayIdentifierForCurrentApplication;
- (BOOL)applicationWithDisplayIdentifierIsBlacklisted:(NSString *)displayIdentifier;
- (void)setApplicationWithDisplayIdentifier:(NSString *)displayIdentifier isBlacklisted:(BOOL)blacklisted;

// Profiles

@property (nonatomic, readonly) NSArray *availableProfileNames;
@property (nonatomic, copy) NSString *currentProfileName;

@end

extern LAActivator *LASharedActivator;

@interface LAActivator (Localization)
- (NSString *)localizedStringForKey:(NSString *)key value:(NSString *)value;

- (NSString *)localizedTitleForEventMode:(NSString *)eventMode;
- (NSString *)localizedTitleForEventName:(NSString *)eventName;
- (NSString *)localizedTitleForListenerName:(NSString *)listenerName;
- (NSString *)localizedTitleForListenerNames:(NSArray *)listenerNames;

- (NSString *)localizedGroupForEventName:(NSString *)eventName;
- (NSString *)localizedGroupForListenerName:(NSString *)listenerName;

- (NSString *)localizedDescriptionForEventMode:(NSString *)eventMode;
- (NSString *)localizedDescriptionForEventName:(NSString *)eventName;
- (NSString *)localizedDescriptionForListenerName:(NSString *)listenerName;
@end

extern NSString * const LAEventModeSpringBoard;
extern NSString * const LAEventModeApplication;
extern NSString * const LAEventModeLockScreen;

extern NSString * const LAActivatorAvailableListenersChangedNotification;
extern NSString * const LAActivatorAvailableEventsChangedNotification;
extern NSString * const LAActivatorAssignmentsChangedNotification;
