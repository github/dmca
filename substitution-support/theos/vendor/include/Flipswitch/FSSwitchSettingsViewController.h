#import <Foundation/Foundation.h>

@protocol FSSwitchSettingsViewController <NSObject> // Must be a UIViewController subclass
@optional
- (id)initWithSwitchIdentifier:(NSString *)switchIdentifier;
@end
