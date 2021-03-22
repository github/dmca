@protocol SBAppSwitcherPeopleDataSourceConsumer;

@protocol SBAppSwitcherPeopleDataSource <NSObject>

@property(assign, nonatomic) id<SBAppSwitcherPeopleDataSourceConsumer> consumer;

- (void)cachedMonogramImageForPersonID:(NSInteger)personID ofSize:(CGFloat)size generatingIfNecessaryWithResult:(id)result;
- (id)contactItemForIndexPath:(NSIndexPath *)indexPath;
- (id)existingCachedMonogramImageForPersonID:(NSInteger)personID ofSize:(CGFloat)size outIsMask:(BOOL *)mask;
- (NSUInteger)numberOfContactsInSection:(NSUInteger)section;
- (NSUInteger)numberOfSections;
- (id)optionalEmptyPlaceholderStringForSection:(NSUInteger)section;
- (void)purgeCaches;
- (id)silhouetteMonogramOfSize:(CGFloat)size;
- (NSString *)titleForSection:(NSUInteger)section;
- (void)updateIfNecessary;

@end
