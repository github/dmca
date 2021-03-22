#ifdef __IPHONE_6_0
#include_next <UIKit/UIActivityViewController.h>
#else
@interface UIActivityViewController : UIViewController

- (instancetype)initWithActivityItems:(NSArray *)items applicationActivities:(NSArray *)applicationActivities;

@end
#endif
