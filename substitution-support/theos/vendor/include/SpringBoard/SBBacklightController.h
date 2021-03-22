@interface SBBacklightController : NSObject

+ (SBBacklightController *)sharedInstance;

@property (readonly, nonatomic) BOOL screenIsOn;

- (void)setIdleTimerDisabled:(BOOL)disabled forReason:(NSString *)reason;

@end