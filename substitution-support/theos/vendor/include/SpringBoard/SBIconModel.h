@class SBApplicationIcon;

@interface SBIconModel : NSObject

- (SBApplicationIcon *)applicationIconForBundleIdentifier:(NSString *)bundleID NS_AVAILABLE_IOS(8_0);
- (SBApplicationIcon *)applicationIconForDisplayIdentifier:(NSString *)displayIdentifier NS_DEPRECATED_IOS(4_0, 8_0);

- (NSArray *)visibleIconIdentifiers;

@property (retain, nonatomic) NSDictionary *leafIconsByIdentifier;

@end
