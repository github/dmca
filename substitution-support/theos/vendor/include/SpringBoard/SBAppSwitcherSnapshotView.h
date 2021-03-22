@class SBDisplayItem, _SBAppSwitcherSnapshotContext, XBApplicationSnapshot;

@interface SBAppSwitcherSnapshotView : UIView

@property (nonatomic, copy, readonly) SBDisplayItem *displayItem;
@property (nonatomic, assign) BOOL shouldTransitionToDefaultPng;

+ (instancetype)appSwitcherSnapshotViewForDisplayItem:(SBDisplayItem *)item orientation:(NSInteger)orientation preferringDownscaledSnapshot:(BOOL)preferDownscale loadAsync:(BOOL)async withQueue:(id)queue;

- (void)_loadSnapshotAsync;
- (void)_loadSnapshotSync;

- (_SBAppSwitcherSnapshotContext *)_contextForAvailableSnapshotWithLayoutState:(id)layoutState preferringDownscaled:(BOOL)preferDownscale defaultImageOnly:(BOOL)defaultOnly;
- (UIImage *)_syncImageFromSnapshot:(XBApplicationSnapshot *)snapshot;

@end
