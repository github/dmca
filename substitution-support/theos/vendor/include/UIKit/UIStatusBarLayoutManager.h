@class UIStatusBarForegroundView, UIStatusBarItemView;

@interface UIStatusBarLayoutManager : NSObject

@property (nonatomic, retain) UIStatusBarForegroundView *foregroundView;
@property (nonatomic, retain, readonly) UIStatusBarItemView *_itemViews;

@property BOOL persistentAnimationsEnabled;

@end
