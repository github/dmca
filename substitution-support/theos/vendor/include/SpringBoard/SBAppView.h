#import <UIKit/UIView.h>

@class SBApplication, FBSDisplay;

@interface SBAppView : UIView

- (instancetype)initWithApp:(SBApplication *)application referenceSize:(CGSize)size orientation:(UIInterfaceOrientation)orientation display:(FBSDisplay *)display hostRequester:(NSString *)requester;
- (instancetype)initWithApp:(SBApplication *)application referenceSize:(CGSize)size orientation:(UIInterfaceOrientation)orientation display:(FBSDisplay *)display;

- (void)setForcesStatusBarHidden:(BOOL)hidden;
- (CGSize)sizeThatFits:(CGSize)size;

@end
