@interface UIActivityViewController (Private)

@property (nonatomic, retain) _UIActivityGroupListViewController *activityGroupListViewController;

- (NSArray *)_availableActivitiesForItems:(NSArray *)items applicationExtensionActivities:(NSArray *)applicationExtensionActivities;

@property (nonatomic, retain) NSArray *activityItems;

@end
