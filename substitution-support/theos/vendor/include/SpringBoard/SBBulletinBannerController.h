@class BBObserver, BBBulletinRequest, BBBulletin;

@interface SBBulletinBannerController : NSObject

+ (instancetype)sharedInstance;

- (void)observer:(BBObserver *)observer addBulletin:(BBBulletin *)bulletin forFeed:(NSUInteger)feed NS_DEPRECATED_IOS(7_0, 8_1);
- (void)observer:(BBObserver *)observer addBulletin:(BBBulletin *)bulletin forFeed:(NSUInteger)feed playLightsAndSirens:(BOOL)playLightsAndSirens withReply:(id)reply NS_AVAILABLE_IOS(8_2);

- (void)showTestBanner NS_DEPRECATED_IOS(5_0, 6_1);

@end
