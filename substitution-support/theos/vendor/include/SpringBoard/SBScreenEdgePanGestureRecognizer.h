#import <UIKit/UIScreenEdgePanGestureRecognizer+Private.h>
#import "SBSystemGestureRecognizerDelegate.h"
#import "UIGestureRecognizer+SpringBoard.h"

@interface SBScreenEdgePanGestureRecognizer : UIScreenEdgePanGestureRecognizer

@property (weak, nonatomic) id<SBSystemGestureRecognizerDelegate> delegate;

@end
