#import <Preferences/PSListItemsController.h>

/**
 * The `HBListItemsController` class in `CepheiPrefs` was used with previous
 * versions to ensure that the tint color from the previous view controller is
 * retained. As of Cephei 1.4, this is no longer needed, and this class is kept
 * for backwards compatibility purposes.
 */

@interface HBListItemsController : PSListItemsController

@end
