@class SBAppSliderScrollingViewController;

@protocol SBAppSliderScrollingViewDelegate

- (NSUInteger)sliderScrollerItemCount:(SBAppSliderScrollingViewController *)sliderScroller;
- (BOOL)sliderScroller:(SBAppSliderScrollingViewController *)sliderScroller isIndexRemovable:(NSUInteger)index;
- (void)sliderScroller:(SBAppSliderScrollingViewController *)sliderScroller itemTapped:(NSUInteger)index;
- (void)sliderScroller:(SBAppSliderScrollingViewController *)sliderScroller itemWantsToBeRemoved:(NSUInteger)index;

@end
