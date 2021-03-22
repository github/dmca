@interface SBLockScreenNotificationListController : UIViewController

- (void)observer:(BBObserver *)observer addBulletin:(BBBulletin *)bulletin forFeed:(NSUInteger)feed;
- (void)observer:(BBObserver *)observer addBulletin:(BBBulletin *)bulletin forFeed:(NSUInteger)feed playLightsAndSirens:(BOOL)playLightsAndSirens withReply:(id)reply;

@end
