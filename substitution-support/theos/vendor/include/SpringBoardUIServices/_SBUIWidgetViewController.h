#import "_SBUIWidgetHost.h"

@interface _SBUIWidgetViewController : UIViewController <_SBUIWidgetHost>

@property (readonly, assign, nonatomic) CGSize preferredViewSize;
@property (readonly, assign, nonatomic) id<_SBUIWidgetHost> widgetHost;

- (void)hostDidDismiss;
- (void)hostWillDismiss;
- (void)hostDidPresent;
- (void)hostWillPresent;

- (void)invalidatePreferredViewSize;

@end
