@protocol SBSystemGestureRecognizerDelegate <UIGestureRecognizerDelegate>

@required

- (UIView *)viewForSystemGestureRecognizer:(UIGestureRecognizer *)recognizer;

@end
