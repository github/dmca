#import "SBFWallpaperView.h"

@interface SBFStaticWallpaperView : SBFWallpaperView

@property (nonatomic, retain, getter=_displayedImage, setter=_setDisplayedImage:) UIImage *displayedImage;

- (UIImage *)snapshotImage;
- (UIImage *)wallpaperImage;

@end
