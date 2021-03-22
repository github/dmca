#import "PLWallpaperImageViewController.h"

@class SBFWallpaperPreviewViewController;

@interface PLStaticWallpaperImageViewController : PLWallpaperImageViewController

@property BOOL allowsEditing;
@property (nonatomic, retain) SBFWallpaperPreviewViewController *wallpaperPreviewViewController;

@end
