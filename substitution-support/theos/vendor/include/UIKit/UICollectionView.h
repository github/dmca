#ifdef __IPHONE_6_0
#include_next <UIKit/UICollectionView.h>
#else
#import "UICollectionViewCell.h"

@interface UICollectionView : UIView

- (NSIndexPath *)indexPathForCell:(UICollectionViewCell *)cell;

@end
#endif
