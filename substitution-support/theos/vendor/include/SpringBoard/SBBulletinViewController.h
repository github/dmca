@class SBNotificationsBulletinInfo, SBNotificationCenterSectionInfo;

@interface SBBulletinViewController : UITableViewController

- (SBNotificationsBulletinInfo *)_bulletinInfoAtIndexPath:(NSIndexPath *)indexPath;
- (SBNotificationCenterSectionInfo *)sectionAtIndex:(NSInteger)index;

@end
