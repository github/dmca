@class SBWallpaperPreviewSnapshotCache, SBFStaticWallpaperView;

@interface SBWallpaperController : NSObject {
    SBWallpaperPreviewSnapshotCache *_previewCache;
    SBFStaticWallpaperView *_lockscreenWallpaperView;
    SBFStaticWallpaperView *_homescreenWallpaperView;
    SBFStaticWallpaperView *_sharedWallpaperView;
}

@property (nonatomic, strong) SBFStaticWallpaperView *lockscreenWallpaperView;
@property (nonatomic, strong) SBFStaticWallpaperView *homescreenWallpaperView;
@property (nonatomic, strong) SBFStaticWallpaperView *sharedWallpaperView;

+ (SBWallpaperController *)sharedInstance;

- (void)beginRequiringWithReason:(NSString *)reason;
- (void)endRequiringWithReason:(NSString *)reason;

@end
