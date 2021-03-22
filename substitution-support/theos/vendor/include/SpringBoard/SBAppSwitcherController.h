#import "SBAppSwitcherScrollingViewDelegate.h"

@class SBAppSwitcherPageViewController;

@interface SBAppSwitcherController : UIViewController <SBAppSwitcherScrollingViewDelegate>

@property (nonatomic, retain) SBAppSwitcherPageViewController *pageController;

@end
