@class SBAwayView;

@interface SBAwayController : NSObject

+ (instancetype)sharedAwayController;

- (void)undim;

@property (nonatomic, retain) SBAwayView *awayView;
@property (readonly) BOOL isDimmed;

@end
