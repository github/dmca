#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

// Listeners represent specific actions that can be performed in response to an event
// Must be registered with LAActivator inside SpringBoard via the registerListener:forName: method

@class LAActivator, LAEvent, UIImage;

@protocol LAListener <NSObject>
@optional

- (void)activator:(LAActivator *)activator didChangeToEventMode:(NSString *)eventMode;

// Incoming events
- (void)activator:(LAActivator *)activator receiveEvent:(LAEvent *)event forListenerName:(NSString *)listenerName; // Normal assigned events
- (void)activator:(LAActivator *)activator abortEvent:(LAEvent *)event forListenerName:(NSString *)listenerName; // Sent when a chorded event gets escalated (short hold becoems a long hold, for example)
- (BOOL)activator:(LAActivator *)activator receiveUnlockingDeviceEvent:(LAEvent *)event forListenerName:(NSString *)listenerName; // Sent at the lock screen when listener is not compatible with event, but potentially is able to unlock the screen to handle it
- (void)activator:(LAActivator *)activator receiveDeactivateEvent:(LAEvent *)event; // Sent when the menu button is pressed. Only handle if you want to suppress the standard menu button behaviour!
- (void)activator:(LAActivator *)activator otherListenerDidHandleEvent:(LAEvent *)event; // Sent when another listener has handled the event
- (void)activator:(LAActivator *)activator receivePreviewEventForListenerName:(NSString *)listenerName; // Sent from the settings pane when a listener is assigned

// Simpler versions
- (void)activator:(LAActivator *)activator receiveEvent:(LAEvent *)event;
- (void)activator:(LAActivator *)activator abortEvent:(LAEvent *)event;

// Metadata (may be cached)
- (NSString *)activator:(LAActivator *)activator requiresLocalizedTitleForListenerName:(NSString *)listenerName;
- (NSString *)activator:(LAActivator *)activator requiresLocalizedDescriptionForListenerName:(NSString *)listenerName;
- (NSString *)activator:(LAActivator *)activator requiresLocalizedGroupForListenerName:(NSString *)listenerName;
- (NSNumber *)activator:(LAActivator *)activator requiresRequiresAssignmentForListenerName:(NSString *)listenerName;
- (NSArray *)activator:(LAActivator *)activator requiresCompatibleEventModesForListenerWithName:(NSString *)listenerName;
- (NSNumber *)activator:(LAActivator *)activator requiresIsCompatibleWithEventName:(NSString *)eventName listenerName:(NSString *)listenerName;
- (NSArray *)activator:(LAActivator *)activator requiresExclusiveAssignmentGroupsForListenerName:(NSString *)listenerName;
- (id)activator:(LAActivator *)activator requiresInfoDictionaryValueOfKey:(NSString *)key forListenerWithName:(NSString *)listenerName;
- (BOOL)activator:(LAActivator *)activator requiresNeedsPoweredDisplayForListenerName:(NSString *)listenerName;

// Icons
//  Fast path that supports scale
- (NSData *)activator:(LAActivator *)activator requiresIconDataForListenerName:(NSString *)listenerName scale:(CGFloat *)scale;
- (NSData *)activator:(LAActivator *)activator requiresSmallIconDataForListenerName:(NSString *)listenerName scale:(CGFloat *)scale;
//  Legacy
- (NSData *)activator:(LAActivator *)activator requiresIconDataForListenerName:(NSString *)listenerName;
- (NSData *)activator:(LAActivator *)activator requiresSmallIconDataForListenerName:(NSString *)listenerName;
//  For cases where PNG data isn't available quickly
- (UIImage *)activator:(LAActivator *)activator requiresIconForListenerName:(NSString *)listenerName scale:(CGFloat)scale;
- (UIImage *)activator:(LAActivator *)activator requiresSmallIconForListenerName:(NSString *)listenerName scale:(CGFloat)scale;

- (id)activator:(LAActivator *)activator requiresGlyphImageDescriptorForListenerName:(NSString *)listenerName;

- (BOOL)activator:(LAActivator *)activator requiresSupportsRemovalForListenerWithName:(NSString *)listenerName;
- (void)activator:(LAActivator *)activator requestsRemovalForListenerWithName:(NSString *)listenerName;
- (NSString *)activator:(LAActivator *)activator requiresConfigurationViewControllerClassNameForListenerWithName:(NSString *)listenerName bundle:(NSBundle **)outBundle; // LAListenerConfigurationViewController
- (id)activator:(LAActivator *)activator requestsConfigurationForListenerWithName:(NSString *)listenerName;
- (void)activator:(LAActivator *)activator didSaveNewConfiguration:(id)configuration forListenerWithName:(NSString *)listenerName;

@end
