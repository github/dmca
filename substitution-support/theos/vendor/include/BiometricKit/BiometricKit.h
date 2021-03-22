#import "BiometricKitDelegate.h"

@interface BiometricKit : NSObject

+ (BiometricKit *)manager;

@property (nonatomic, assign) id<BiometricKitDelegate> delegate;

- (NSArray *)identities:(id)object;

- (BOOL)isTouchIDCapable;

@end
