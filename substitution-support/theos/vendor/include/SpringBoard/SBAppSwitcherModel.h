@class SBDisplayItem, SBDisplayLayout;

@interface SBAppSwitcherModel : NSObject

+ (instancetype)sharedInstance;

- (void)addToFront:(SBDisplayLayout *)layout NS_DEPRECATED_IOS(4_0, 9_0);
- (NSArray *)snapshotOfFlattenedArrayOfAppIdentifiersWhichIsOnlyTemporary NS_DEPRECATED_IOS(8_0, 9_0);
- (void)removeDisplayItem:(SBDisplayItem *)item NS_DEPRECATED_IOS(8_0, 9_0)

- (void)addToFront:(SBDisplayItem *)item role:(NSInteger)role NS_AVAILABLE_IOS(9_0);

- (void)remove:(SBDisplayItem *)item;

- (NSArray *)mainSwitcherDisplayItems NS_AVAILABLE_IOS(9_0);

@end
