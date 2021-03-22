#import "HBListController.h"

@class PSSpecifier;

NS_ASSUME_NONNULL_BEGIN

@interface HBListController (Actions)

- (void)hb_respring:(PSSpecifier *)specifier;
- (void)hb_respringAndReturn:(PSSpecifier *)specifier;

- (void)hb_openURL:(PSSpecifier *)specifier;

@end

NS_ASSUME_NONNULL_END
