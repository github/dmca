@class BBObserver, BBBulletin;

@interface SBBulletinObserverViewController : UIViewController

- (void)observer:(BBObserver *)observer addBulletin:(BBBulletin *)bulletin forFeed:(NSUInteger)feed;

@end
