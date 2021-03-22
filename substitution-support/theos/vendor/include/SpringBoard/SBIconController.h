@class SBDockIconListView, SBIconModel, SBRootIconListView, SBRootFolderController, SBIconViewMap, SBIconContentView, SBRootFolder, SBIconListView, SBFolder;

@interface SBIconController : NSObject {
	SBIconModel *_iconModel;
}

+ (SBIconController *)sharedInstance;

@property (nonatomic, retain) SBIconModel *model;

@property (nonatomic, readonly) SBIconViewMap *homescreenIconViewMap;

@property (nonatomic, retain, readonly) SBRootIconListView *currentRootIconList;
@property (nonatomic, retain, readonly) SBDockIconListView *dockListView;
@property (nonatomic, retain, readonly) SBIconContentView *contentView;

@property (nonatomic, retain, readonly) SBRootFolderController *_rootFolderController;
@property (nonatomic, retain, readonly) SBRootFolder *rootFolder;

- (BOOL)isEditing;
- (void)setIsEditing:(BOOL)editing;
- (void)setIsEditing:(BOOL)editing withFeedbackBehavior:(id)behavior;

- (void)getListView:(SBIconListView **)listView folder:(SBFolder *)folder relativePath:(id)path forIndexPath:(NSIndexPath *)indexPath createIfNecessary:(BOOL)create; // FIXME: What is path?

@end
