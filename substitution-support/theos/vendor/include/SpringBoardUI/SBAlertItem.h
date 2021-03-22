#import "_SBAlertController.h"

@interface SBAlertItem : NSObject

+ (void)activateAlertItem:(SBAlertItem *)alertItem;

- (void)configure:(BOOL)configure requirePasscodeForActions:(BOOL)requirePasscode;
- (void)dismiss;

- (void)deactivateForButton;

@property (nonatomic, retain) UIAlertView *alertSheet;

- (_SBAlertController *)alertController;

@end
