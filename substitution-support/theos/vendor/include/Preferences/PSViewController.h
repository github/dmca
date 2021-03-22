#import <UIKit/UIKit.h>

@class PSSpecifier;

@interface PSViewController : UIViewController

- (instancetype)initForContentSize:(CGSize)contentSize;

- (void)pushController:(PSViewController *)controller;
- (void)pushController:(PSViewController *)controller animate:(BOOL)animated;

- (void)showController:(PSViewController *)controller;
- (void)showController:(PSViewController *)controller animate:(BOOL)animated;

- (id)readPreferenceValue:(PSSpecifier *)specifier;
- (void)setPreferenceValue:(id)value specifier:(PSSpecifier *)specifier;

@property (nonatomic, retain) PSSpecifier *specifier;
@property (nonatomic, retain) PSViewController *parentController;
@property (nonatomic, retain) PSViewController *rootController;

- (void)suspend;

- (void)willBecomeActive;
- (void)willResignActive;
- (void)willUnlock;

- (void)didLock;
- (void)didUnlock;
- (void)didWake;

- (void)formSheetViewDidDisappear;
- (void)formSheetViewWillDisappear;

- (void)popupViewDidDisappear;
- (void)popupViewWillDisappear;

- (void)handleURL:(NSURL *)url;
- (void)statusBarWillAnimateByHeight:(float)arg1;

- (BOOL)canBeShownFromSuspendedState;

@end
