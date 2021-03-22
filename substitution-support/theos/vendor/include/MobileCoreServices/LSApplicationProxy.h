#import "LSBundleProxy.h"

@interface LSApplicationProxy : LSBundleProxy

+ (instancetype)applicationProxyForIdentifier:(NSString *)identifier;

@property (nonatomic, retain) NSString *applicationIdentifier;

@property (readonly) BOOL isInstalled;

@end
