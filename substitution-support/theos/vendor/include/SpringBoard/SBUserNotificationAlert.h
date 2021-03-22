#import <SpringBoardUI/SBAlertItem.h>

@interface SBUserNotificationAlert : SBAlertItem

@property (nonatomic, retain) NSString *alertHeader;
@property (nonatomic, retain) NSString *defaultButtonTitle;
@property (nonatomic, retain) NSString *alternateButtonTitle;
@property (nonatomic, retain) NSString *otherButtonTitle;

@end
