@protocol BBWeeAppController

- (UIView *)view;

@optional
@property (nonatomic, retain) id host;
- (void)viewWillDisappear;
- (void)viewDidAppear;
- (void)setPresentationView:(id)presentationView;
- (float)presentationHeight;
- (void)unloadPresentationController;
- (id)presentationControllerForMode:(int)mode;
- (id)launchURLForTapLocation:(CGPoint)tapLocation;
- (id)launchURL;
- (void)loadView;
- (void)clearShapshotImage;
- (void)unloadView;
- (void)loadFullView;
- (void)loadPlaceholderView;
- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (void)viewDidDisappear;
- (void)viewWillAppear;
- (float)viewHeight;

@end
