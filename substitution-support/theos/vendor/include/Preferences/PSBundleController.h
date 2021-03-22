@class PSListController, PSSpecifier;

@interface PSBundleController : NSObject

- (instancetype)initWithParentListController:(PSListController *)listController;
- (instancetype)initWithParentListController:(PSListController *)listController properties:(NSDictionary *)properties;

- (NSArray *)specifiersWithSpecifier:(PSSpecifier *)specifier;

- (void)load;
- (void)unload;

@end
