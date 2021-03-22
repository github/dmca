#import <UIKit/UIKit.h>
#import <libkern/OSAtomic.h>

@class ALApplicationList;

@interface ALApplicationTableDataSource : NSObject <UITableViewDataSource> {
@private
	NSMutableArray *_sectionDescriptors;
	UITableView *_tableView;
	NSBundle *_localizationBundle;
	BOOL _loadsAsynchronously;
}

+ (NSArray *)standardSectionDescriptors;

+ (id)dataSource;
- (id)init;

@property (nonatomic, copy) NSArray *sectionDescriptors;
@property (nonatomic, retain) UITableView *tableView;
@property (nonatomic, retain) NSBundle *localizationBundle;
@property (nonatomic, assign) BOOL loadsAsynchronously;

- (id)cellDescriptorForIndexPath:(NSIndexPath *)indexPath; // NSDictionary if custom cell; NSString if app cell; nil if loading
- (NSString *)displayIdentifierForIndexPath:(NSIndexPath *)indexPath;

- (void)insertSectionDescriptor:(NSDictionary *)sectionDescriptor atIndex:(NSInteger)index;
- (void)removeSectionDescriptorAtIndex:(NSInteger)index;
- (void)removeSectionDescriptorsAtIndexes:(NSIndexSet *)indexSet;

- (BOOL)waitUntilDate:(NSDate *)date forContentInSectionAtIndex:(NSInteger)sectionIndex;

@end

extern const NSString *ALSectionDescriptorTitleKey;
extern const NSString *ALSectionDescriptorFooterTitleKey;
extern const NSString *ALSectionDescriptorPredicateKey;
extern const NSString *ALSectionDescriptorCellClassNameKey;
extern const NSString *ALSectionDescriptorIconSizeKey;
extern const NSString *ALSectionDescriptorSuppressHiddenAppsKey;
extern const NSString *ALSectionDescriptorVisibilityPredicateKey;

extern const NSString *ALItemDescriptorTextKey;
extern const NSString *ALItemDescriptorDetailTextKey;
extern const NSString *ALItemDescriptorImageKey;
