/**
 * The `PSListController+HBTintAdditions` class category in `CepheiPrefs`
 * provides a property for setting the desired appearance settings of the view
 * controller.
 */

#import <Preferences/PSListController.h>

@class HBAppearanceSettings;

NS_ASSUME_NONNULL_BEGIN

@interface PSListController (HBTintAdditions)

/**
 * The appearance settings for the view controller.
 *
 * This should only be set in an init or viewDidLoad method of the view
 * controller. The result when this property or its properties are changed after
 * the view has appeared is undefined.
 */
@property (nonatomic, copy, nullable, setter=hb_setAppearanceSettings:) HBAppearanceSettings *hb_appearanceSettings;

@end

NS_ASSUME_NONNULL_END
