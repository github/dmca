#import "LSBundleProxy.h"

@interface LSPlugInKitProxy : LSBundleProxy

@property (nonatomic, readonly) LSBundleProxy *containingBundle;
@property (nonatomic, readonly) NSDictionary *infoPlist;
@property (nonatomic, readonly) NSString *pluginIdentifier;

@end
