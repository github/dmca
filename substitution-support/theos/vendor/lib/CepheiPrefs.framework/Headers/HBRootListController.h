#import "HBListController.h"

/**
 * The `HBRootListController` class in `CepheiPrefs` provides a list controller
 * class that should be used as the root of the package's settings. It includes
 * two class methods you can override to provide a default message and a URL
 * that the user can share via a sharing button displayed to the right of the
 * navigation bar.
 *
 * It is recommended that you use this class even if its current features
 * aren't appealing in case of future improvements or code that relies on the
 * presence of an HBRootListController.
 */

NS_ASSUME_NONNULL_BEGIN

@interface HBRootListController : HBListController

/**
 * @name Constants
 */

/**
 * A string to be used as a default message when the user shares the package
 * to a friend or social website. Override this method to return your own
 * string.
 *
 * If the return value of this method and hb_shareURL are nil, the sharing
 * button will not be displayed.
 *
 * @returns By default, nil.
 */
+ (nullable NSString *)hb_shareText;


/**
 * The URL to be shared when the user shares the package to a friend or social
 * website. Override this method to return your own URL.
 *
 * If the return value of this method and hb_shareText are nil, the sharing
 * button will not be displayed.
 *
 * @returns By default, nil.
 */
+ (nullable NSURL *)hb_shareURL;

@end

NS_ASSUME_NONNULL_END
