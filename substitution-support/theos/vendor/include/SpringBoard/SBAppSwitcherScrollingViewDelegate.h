@class SBAppSwitcherPageViewController, SBDisplayItem, SBDisplayLayout;

@protocol SBAppSwitcherScrollingViewDelegate

- (_Bool)switcherScroller:(SBAppSwitcherPageViewController *)switcherScroller isDisplayItemRemovable:(SBDisplayItem *)displayItem;
- (void)switcherScroller:(SBAppSwitcherPageViewController *)switcherScroller itemTapped:(SBDisplayLayout *)displayLayout;
- (void)switcherScroller:(SBAppSwitcherPageViewController *)switcherScroller displayItemWantsToBeRemoved:(SBDisplayItem *)displayItem;

@end
