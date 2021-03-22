#import <MobileIcons/MobileIcons.h>
#include <sys/cdefs.h>

@class LSApplicationProxy;

__BEGIN_DECLS

UIImage *_UIImageWithName(NSString *name);

__END_DECLS

@interface UIImage (Private)

+ (instancetype)kitImageNamed:(NSString *)name;
+ (instancetype)imageNamed:(NSString *)name inBundle:(NSBundle *)bundle;

+ (instancetype)imageWithContentsOfCPBitmapFile:(NSString *)filename flags:(NSInteger)flags; // TODO: make this an enum

+ (instancetype)_applicationIconImageForBundleIdentifier:(NSString *)bundleIdentifier format:(MIIconVariant)format scale:(CGFloat)scale;
+ (instancetype)_iconForResourceProxy:(LSApplicationProxy *)applicationProxy format:(MIIconVariant)format;

- (instancetype)_applicationIconImageForFormat:(MIIconVariant)format precomposed:(BOOL)precomposed scale:(CGFloat)scale;

- (instancetype)_flatImageWithColor:(UIColor *)color;

- (BOOL)writeToCPBitmapFile:(NSString *)filename flags:(NSInteger)flags; // TODO: make this an enum

@property CGFloat scale;

@end
