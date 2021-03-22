#import "CKTypingIndicatorLayer.h"

@interface CKTypingView : UIView

@property (nonatomic, readonly, retain) CKTypingIndicatorLayer *layer; // 7.0(?) – 9.3
@property (nonatomic, retain) CKTypingIndicatorLayer *indicatorLayer; // 10.0+

@end
