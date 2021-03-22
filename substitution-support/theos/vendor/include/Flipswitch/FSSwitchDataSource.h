#import <CoreGraphics/CoreGraphics.h>
#import "FSSwitchState.h"
#import "FSSwitchSettingsViewController.h"

@class UIColor;

@protocol FSSwitchDataSource <NSObject>
@optional

- (FSSwitchState)stateForSwitchIdentifier:(NSString *)switchIdentifier;
// Gets the current state of the switch.
// Must override if building a settings-like switch.
// Return FSSwitchStateIndeterminate if switch is loading
// By default returns FSSwitchStateIndeterminate

- (void)applyState:(FSSwitchState)newState forSwitchIdentifier:(NSString *)switchIdentifier;
// Sets the new state of the switch
// Must override if building a settings-like switch.
// By default calls through to applyActionForSwitchIdentifier: if newState is different from the current state

- (void)applyActionForSwitchIdentifier:(NSString *)switchIdentifier;
// Runs the default action for the switch.
// Must override if building an action-like switch.
// By default calls through to applyState:forSwitchIdentifier: if state is not indeterminate

- (NSString *)titleForSwitchIdentifier:(NSString *)switchIdentifier;
// Returns the localized title for the switch.
// By default reads the CFBundleDisplayName out of the switch's bundle.

- (id)glyphImageDescriptorOfState:(FSSwitchState)switchState size:(CGFloat)size scale:(CGFloat)scale forSwitchIdentifier:(NSString *)switchIdentifier;
// Provide an image descriptor that best displays at the requested size and scale
// By default looks through the bundle to find a glyph image

- (id)glyphImageDescriptorOfState:(FSSwitchState)switchState variant:(NSString *)variant size:(CGFloat)size scale:(CGFloat)scale forSwitchIdentifier:(NSString *)switchIdentifier;
// Provide a modern image descriptor that best displays at the requested size and scale (1.0.4)
// By default looks through the bundle to find a modern glyph image, falling back to using the standard glyph image descriptor

- (UIColor *)primaryColorForSwitchIdentifier:(NSString *)switchIdentifier;
// Provide a primary branding color that is associated the the product, service or feature provided by the switch
// By default looks through the bundle to find a color specified in the primary-color Info.plist key, falling back to nil if unspecified

- (NSBundle *)bundleForSwitchIdentifier:(NSString *)switchIdentifier;
// Provides a bundle to look for localizations/images in
// By default returns the bundle for the current class

- (void)switchWasRegisteredForIdentifier:(NSString *)switchIdentifier;
// Called when switch is first registered

- (void)switchWasUnregisteredForIdentifier:(NSString *)switchIdentifier;
// Called when switch is unregistered

- (BOOL)hasAlternateActionForSwitchIdentifier:(NSString *)switchIdentifier;
// Gets whether the switch supports an alternate or "hold" action
// By default queries if switch responds to applyAlternateActionForSwitchIdentifier: or if it has a "alternate-action-url" key set

- (void)applyAlternateActionForSwitchIdentifier:(NSString *)switchIdentifier;
// Applies the alternate or "hold" action
// By default launches the URL stored in the "alternate-action-url" key of the switch's bundle

- (BOOL)switchWithIdentifierIsEnabled:(NSString *)switchIdentifier;
// Gets whether or not the switch is enabled
// Must override if switch can be disabled
// By default returns YES

- (void)beginPrewarmingForSwitchIdentifier:(NSString *)switchIdentifier;
// Prepares switch for state changes (1.0.3 or later)

- (void)cancelPrewarmingForSwitchIdentifier:(NSString *)switchIdentifier;
// Cancels previously requested prewarm (1.0.3 or later)

- (NSString *)descriptionOfState:(FSSwitchState)state forSwitchIdentifier:(NSString *)switchIdentifier;
// Requests a description of a specific state, in the context of a specific switch identifier (1.0.3 or later)

- (Class <FSSwitchSettingsViewController>)settingsViewControllerClassForSwitchIdentifier:(NSString *)switchIdentifier;
// Requests the class to handle configuration for the specified switch (1.0.3)

- (BOOL)switchWithIdentifierIsSimpleAction:(NSString *)switchIdentifier;
// Gets whether or not the switch is a simple "button" (1.0.4)

@end
