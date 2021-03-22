#ifdef __IPHONE_6_0
#include_next <UIKit/UITableViewHeaderFooterView.h>
#else
@interface UITableViewHeaderFooterView : UITableViewCell

@end
#endif
