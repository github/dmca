#ifdef __IPHONE_6_0
#include_next <UIKit/UICollectionViewCell.h>
#else
@interface UICollectionViewCell : UIView

@end
#endif
