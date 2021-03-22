#import <UIKit/UIControl.h>
#import "FSSwitchState.h"
#import "FSSwitchSettingsViewController.h"

@class UIButton;

@interface FSSwitchPanel : NSObject

+ (FSSwitchPanel *)sharedPanel;

@property (nonatomic, readonly, copy) NSArray *switchIdentifiers;
// Returns a list of identifying all switches installed on the device

@property (nonatomic, readonly, copy) NSArray *sortedSwitchIdentifiers;
// Returns a list of identifying all switches installed on the device sorted by localized title (1.0.3 or later)

- (NSString *)titleForSwitchIdentifier:(NSString *)switchIdentifier;
// Returns the localized title for a specific switch

- (UIButton *)buttonForSwitchIdentifier:(NSString *)switchIdentifier usingTemplate:(NSBundle *)templateBundle;
// Returns a UIButton for a specific switch
// The button automatically updates its style based on the user interaction and switch state changes, applies the standard action when pressed, and applies the alternate action when held

- (UIImage *)imageOfSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState forSwitchIdentifier:(NSString *)switchIdentifier usingTemplate:(NSBundle *)templateBundle;
- (UIImage *)imageOfSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState scale:(CGFloat)scale forSwitchIdentifier:(NSString *)switchIdentifier usingTemplate:(NSBundle *)templateBundle;
// Returns an image representing how a specific switch would look in a particular state when styled with the provided template

- (BOOL)hasCachedImageOfSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState forSwitchIdentifier:(NSString *)switchIdentifier usingTemplate:(NSBundle *)templateBundle;
- (BOOL)hasCachedImageOfSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState scale:(CGFloat)scale forSwitchIdentifier:(NSString *)switchIdentifier usingTemplate:(NSBundle *)templateBundle;
// Returns YES if an image representing the state is loaded

- (FSSwitchState)stateForSwitchIdentifier:(NSString *)switchIdentifier;
// Returns the current state of a particular switch
- (void)setState:(FSSwitchState)state forSwitchIdentifier:(NSString *)switchIdentifier;
// Updates the state of a particular switch. If the switch accepts the change it will send a state change
- (void)applyActionForSwitchIdentifier:(NSString *)switchIdentifier;
// Applies the default action of a particular switch

- (BOOL)hasAlternateActionForSwitchIdentifier:(NSString *)switchIdentifier;
// Queries whether a switch supports an alternate action. This is often triggered by a hold gesture
- (void)applyAlternateActionForSwitchIdentifier:(NSString *)switchIdentifier;
// Apply the alternate action of a particular switch

- (void)openURLAsAlternateAction:(NSURL *)url;
// Helper method to open a particular URL as if it were launched from an alternate action

- (BOOL)switchWithIdentifierIsEnabled:(NSString *)switchIdentifier;
// Returns YES if the switch is enabled (1.0.1 or later)

- (void)beginPrewarmingForSwitchIdentifier:(NSString *)switchIdentifier;
// Prepares switch for state changes (1.0.3 or later)

- (void)cancelPrewarmingForSwitchIdentifier:(NSString *)switchIdentifier;
// Cancels previously requested prewarm (1.0.3 or later)

- (NSString *)descriptionOfState:(FSSwitchState)state forSwitchIdentifier:(NSString *)switchIdentifier;
// Requests a description of a specific state, in the context of a specific switch identifier (1.0.3 or later)

- (Class <FSSwitchSettingsViewController>)settingsViewControllerClassForSwitchIdentifier:(NSString *)switchIdentifier;
// Retrieves the settings view controller class for a specific switch identifier (1.0.3 or later)

- (UIViewController <FSSwitchSettingsViewController> *)settingsViewControllerForSwitchIdentifier:(NSString *)switchIdentifier;
// Retrieves a settings view controller for a specific switch identifier (1.0.3 or later)

- (BOOL)switchWithIdentifierIsSimpleAction:(NSString *)switchIdentifier;
// Gets whether or not the switch is a simple "button" (1.0.4 or later)

- (UIImage *)imageOfSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState forSwitchIdentifier:(NSString *)switchIdentifier usingLayerSet:(NSString *)layerSet inTemplate:(NSBundle *)templateBundle;
- (UIImage *)imageOfSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState scale:(CGFloat)scale forSwitchIdentifier:(NSString *)switchIdentifier usingLayerSet:(NSString *)layerSet inTemplate:(NSBundle *)templateBundle;
// Returns an image representing how a specific switch would look in a particular state when styled with the provided template

- (BOOL)hasCachedImageOfSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState forSwitchIdentifier:(NSString *)switchIdentifier usingLayerSet:(NSString *)layerSet inTemplate:(NSBundle *)templateBundle;
- (BOOL)hasCachedImageOfSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState scale:(CGFloat)scale forSwitchIdentifier:(NSString *)switchIdentifier usingLayerSet:(NSString *)layerSet inTemplate:(NSBundle *)templateBundle;
// Returns YES if an image representing the state is loaded

- (UIColor *)primaryColorForSwitchIdentifier:(NSString *)switchIdentifier;
// Returns the primary branding color that is associated the the product, service or feature provided by the switch, if any

@end

@protocol FSSwitchDataSource;

@interface FSSwitchPanel (SpringBoard)
- (void)registerDataSource:(id<FSSwitchDataSource>)dataSource forSwitchIdentifier:(NSString *)switchIdentifier;
// Registers a switch implementation for a specific identifier. Bundlee in /Library/Switches will have their principal class automatically loaded
- (void)unregisterSwitchIdentifier:(NSString *)switchIdentifier;
// Unregisters a switch
- (void)stateDidChangeForSwitchIdentifier:(NSString *)switchIdentifier;
// Informs the system when a switch changes its state. This will trigger any switch buttons to update their style
@end

extern NSString * const FSSwitchPanelSwitchesChangedNotification;

extern NSString * const FSSwitchPanelSwitchStateChangedNotification;
extern NSString * const FSSwitchPanelSwitchIdentifierKey;

extern NSString * const FSSwitchPanelSwitchWillOpenURLNotification;

@interface FSSwitchPanel (LayerEffects)
- (void)applyEffectsToLayer:(CALayer *)layer forSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState usingTemplate:(NSBundle *)templateBundle;
- (void)applyEffectsToLayer:(CALayer *)layer forSwitchState:(FSSwitchState)state controlState:(UIControlState)controlState usingLayerSet:(NSString *)layerSet inTemplate:(NSBundle *)templateBundle;
@end
