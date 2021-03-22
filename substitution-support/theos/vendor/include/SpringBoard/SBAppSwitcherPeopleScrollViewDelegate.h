@class SBAppSwitcherPeopleScrollView, SBScrollViewItemWrapper;

@protocol SBAppSwitcherPeopleScrollViewDelegate <UIScrollViewDelegate>

- (NSInteger)numberOfSectionsInPeopleScrollView:(SBAppSwitcherPeopleScrollView *)peopleScrollView;
- (SBScrollViewItemWrapper *)peopleScrollView:(SBAppSwitcherPeopleScrollView *)peopleScrollView itemAtIndexPath:(NSIndexPath *)indexPath;
- (NSInteger)peopleScrollView:(SBAppSwitcherPeopleScrollView *)peopleScrollView numberOfItemsInSection:(NSInteger)section;
- (NSString *)peopleScrollView:(SBAppSwitcherPeopleScrollView *)peopleScrollView placeholderStringForEmptySection:(NSUInteger)emptySection;
- (CGSize)peopleScrollView:(SBAppSwitcherPeopleScrollView *)peopleScrollView sizeForItem:(SBScrollViewItemWrapper *)item expanded:(BOOL)expanded;
- (CGSize)peopleScrollView:(SBAppSwitcherPeopleScrollView *)peopleScrollView sizeForPlaceholderForSection:(NSInteger)section;
- (NSString *)peopleScrollView:(SBAppSwitcherPeopleScrollView *)peopleScrollView titleForSection:(NSUInteger)section;
- (UIView *)peopleScrollView:(SBAppSwitcherPeopleScrollView *)peopleScrollView viewForItem:(SBScrollViewItemWrapper *)item;

@end
