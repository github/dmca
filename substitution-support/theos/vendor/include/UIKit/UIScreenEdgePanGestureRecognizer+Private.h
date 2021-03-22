#import <UIKit/UIScreenEdgePanGestureRecognizer.h>
#import "_UIScreenEdgePanRecognizer.h"

@interface UIScreenEdgePanGestureRecognizer (Private) 

- (instancetype)initWithTarget:(id)target action:(SEL)action type:(UIScreenEdgePanRecognizerType)type;

@end