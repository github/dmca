typedef NS_ENUM(NSUInteger, UIStatusBarItemType) {
	UIStatusBarItemTypeDontKnowYetComeBackLater
};

@class UIStatusBarItemView, UIStatusBarLayoutManager;

@interface UIStatusBarItem : NSObject

@property (nonatomic, readonly) NSString *indicatorName;

- (instancetype)initWithType:(UIStatusBarItemType)type;

- (void)setView:(UIStatusBarItemView *)view forManager:(UIStatusBarLayoutManager *)layoutManager;

@property UIStatusBarItemType type;

@end
