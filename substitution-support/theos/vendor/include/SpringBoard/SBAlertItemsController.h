@class SBAlertItem;

@interface SBAlertItemsController : NSObject

+ (instancetype)sharedInstance;

- (void)activateAlertItem:(SBAlertItem *)alertItem;

@end
