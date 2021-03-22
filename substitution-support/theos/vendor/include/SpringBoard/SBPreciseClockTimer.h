@interface SBPreciseClockTimer : NSObject

+ (NSDate *)now;

+ (instancetype)sharedInstance;

- (NSNumber *)startMinuteUpdatesWithHandler:(void(^)())handler;
- (void)stopMinuteUpdatesForToken:(NSNumber *)token;

@end