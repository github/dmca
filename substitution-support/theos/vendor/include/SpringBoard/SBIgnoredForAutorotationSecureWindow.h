#import "SBSecureWindow.h"

@interface SBIgnoredForAutorotationSecureWindow : SBSecureWindow

- (BOOL)_shouldControlAutorotation;

@end