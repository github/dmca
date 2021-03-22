#import "PSViewController.h"

@class PSRootController, PSSpecifier, PSTableCell;

@interface PSListController : PSViewController <UITableViewDelegate, UITableViewDataSource, UIAppearance> {
	NSMutableArray *_specifiers;
}

+ (BOOL)displaysButtonBar;

- (NSMutableArray *)loadSpecifiersFromPlistName:(NSString *)name target:(PSListController *)target;
- (NSMutableArray *)loadSpecifiersFromPlistName:(NSString *)name target:(PSListController *)target bundle:(NSBundle *)bundle;

- (Class)tableViewClass;
- (UITableViewStyle)tableViewStyle;

@property (nonatomic, retain) UITableView *view;
@property (nonatomic, retain) UITableView *table; // 3.0 - 6.0
@property (nonatomic, retain) UITableView *tableView;

- (PSSpecifier *)specifierAtIndex:(NSInteger)index;
- (PSSpecifier *)specifierAtIndexPath:(NSIndexPath *)indexPath;
- (PSSpecifier *)specifierForID:(NSString *)identifier;
- (NSArray *)specifiersForIDs:(NSArray *)identifiers;
- (NSArray *)specifiersInGroup:(NSInteger)group;

@property (nonatomic, retain) NSMutableArray *specifiers;
@property (nonatomic, retain) PSSpecifier *specifier;
@property (nonatomic, retain) NSString *specifierID;
@property (nonatomic, retain) NSString *specifierIDPendingPush;
@property (nonatomic, retain) id specifierDataSource;

- (NSInteger)numberOfGroups;
- (NSInteger)rowsForGroup:(NSInteger)group;

- (BOOL)getGroup:(NSInteger *)group row:(NSInteger *)row ofSpecifier:(PSSpecifier *)specifier;
- (BOOL)getGroup:(NSInteger *)group row:(NSInteger *)row ofSpecifierAtIndex:(NSInteger)specifier;
- (BOOL)getGroup:(NSInteger *)group row:(NSInteger *)row ofSpecifierID:(NSString *)specifierID;

- (NSInteger)indexForIndexPath:(NSIndexPath *)indexPath;
- (NSInteger)indexForRow:(NSInteger)row inGroup:(NSInteger)group;

- (NSInteger)indexOfGroup:(NSInteger)group;
- (NSInteger)indexOfSpecifier:(PSSpecifier *)specifier;
- (NSInteger)indexOfSpecifierID:(NSString *)specifierID;

- (NSIndexPath *)indexPathForIndex:(NSInteger)index;
- (NSIndexPath *)indexPathForSpecifier:(PSSpecifier *)specifier;

- (void)addSpecifier:(PSSpecifier *)specifier;
- (void)addSpecifier:(PSSpecifier *)specifier animated:(BOOL)animated;
- (void)addSpecifiersFromArray:(NSArray *)specifiers;
- (void)addSpecifiersFromArray:(NSArray *)specifiers animated:(BOOL)animated;

- (void)insertSpecifier:(PSSpecifier *)specifier afterSpecifier:(PSSpecifier *)afterSpecifier;
- (void)insertSpecifier:(PSSpecifier *)specifier afterSpecifier:(PSSpecifier *)afterSpecifier animated:(BOOL)animated;
- (void)insertSpecifier:(PSSpecifier *)specifier afterSpecifierID:(NSString *)specifierID;
- (void)insertSpecifier:(PSSpecifier *)specifier afterSpecifierID:(NSString *)specifierID animated:(BOOL)animated;
- (void)insertSpecifier:(PSSpecifier *)specifier atEndOfGroup:(NSInteger)groupIndex;
- (void)insertSpecifier:(PSSpecifier *)specifier atEndOfGroup:(NSInteger)groupIndex animated:(BOOL)animated;
- (void)insertSpecifier:(PSSpecifier *)specifier atIndex:(NSInteger)index;
- (void)insertSpecifier:(PSSpecifier *)specifier atIndex:(NSInteger)index animated:(BOOL)animated;

- (void)insertContiguousSpecifiers:(NSArray *)specifiers afterSpecifier:(PSSpecifier *)specifier;
- (void)insertContiguousSpecifiers:(NSArray *)specifiers afterSpecifier:(PSSpecifier *)specifier animated:(BOOL)animated;
- (void)insertContiguousSpecifiers:(NSArray *)specifiers afterSpecifierID:(NSString *)specifierID;
- (void)insertContiguousSpecifiers:(NSArray *)specifiers afterSpecifierID:(NSString *)specifierID animated:(BOOL)animated;
- (void)insertContiguousSpecifiers:(NSArray *)specifiers atEndOfGroup:(NSInteger)groupIndex;
- (void)insertContiguousSpecifiers:(NSArray *)specifiers atEndOfGroup:(NSInteger)groupIndex animated:(BOOL)animated;
- (void)insertContiguousSpecifiers:(NSArray *)specifiers atIndex:(NSInteger)index;
- (void)insertContiguousSpecifiers:(NSArray *)specifiers atIndex:(NSInteger)index animated:(BOOL)animated;

- (void)reload;
- (void)reloadSpecifier:(PSSpecifier *)specifier;
- (void)reloadSpecifier:(PSSpecifier *)specifier animated:(BOOL)animated;
- (void)reloadSpecifierAtIndex:(NSInteger)index;
- (void)reloadSpecifierAtIndex:(NSInteger)index animated:(BOOL)animated;
- (void)reloadSpecifierID:(NSString *)specifierID;
- (void)reloadSpecifierID:(NSString *)specifierID animated:(BOOL)animated;
- (void)reloadSpecifiers;

- (void)removeContiguousSpecifiers:(NSArray *)specifiers;
- (void)removeContiguousSpecifiers:(NSArray *)specifiers animated:(BOOL)animated;
- (void)removeLastSpecifier;
- (void)removeLastSpecifierAnimated:(BOOL)animated;
- (void)removeSpecifier:(PSSpecifier *)specifier;
- (void)removeSpecifier:(PSSpecifier *)specifier animated:(BOOL)animated;
- (void)removeSpecifierAtIndex:(NSInteger)index;
- (void)removeSpecifierAtIndex:(NSInteger)index animated:(BOOL)animated;
- (void)removeSpecifierID:(NSString *)specifierID;
- (void)removeSpecifierID:(NSString *)specifierID animated:(BOOL)animated;

- (void)replaceContiguousSpecifiers:(NSArray *)specifiers withSpecifiers:(NSArray *)newSpecifiers;
- (void)replaceContiguousSpecifiers:(NSArray *)specifiers withSpecifiers:(NSArray *)newSpecifiers animated:(BOOL)animated;

- (void)updateSpecifiers:(NSArray *)specifiers withSpecifiers:(NSArray *)newSpecifiers;
- (void)updateSpecifiersInRange:(NSRange)range withSpecifiers:(NSArray *)newSpecifiers;

- (NSRange)rangeOfSpecifiersInGroupID:(NSString *)groupID;

- (PSTableCell *)cachedCellForSpecifier:(PSSpecifier *)specifier;
- (PSTableCell *)cachedCellForSpecifierID:(NSString *)specifierID;

@property (nonatomic, retain) NSBundle *bundle;

- (void)reloadIconForSpecifierForBundle:(NSBundle *)bundle;

@property (nonatomic) BOOL forceSynchronousIconLoadForCreatedCells;

@property (nonatomic, retain) UIColor *altTextColor;
@property (nonatomic, retain) UIColor *backgroundColor;
@property (nonatomic, retain) UIColor *buttonTextColor;
@property (nonatomic, retain) UIColor *cellAccessoryColor;
@property (nonatomic, retain) UIColor *cellAccessoryHighlightColor;
@property (nonatomic, retain) UIColor *cellHighlightColor;
@property (nonatomic, retain) UIColor *editableInsertionPointColor;
@property (nonatomic, retain) UIColor *editablePlaceholderTextColor;
@property (nonatomic, retain) UIColor *editableSelectionBarColor;
@property (nonatomic, retain) UIColor *editableSelectionHighlightColor;
@property (nonatomic, retain) UIColor *editableTextColor;
@property (nonatomic, retain) UIColor *footerHyperlinkColor;
@property (nonatomic, retain) UIColor *foregroundColor;
@property (nonatomic, retain) UIColor *segmentedSliderTrackColor;
@property (nonatomic, retain) UIColor *separatorColor;
@property (nonatomic, retain) UIColor *textColor;

@property (nonatomic) BOOL usesDarkTheme;
@property (nonatomic) BOOL edgeToEdgeCells;
@property (nonatomic) BOOL resusesCells;
@property (nonatomic, readonly) NSInteger observerType; // TODO: what is this?

@property (nonatomic, retain) NSDictionary *pendingURLResourceDictionary;

@end
