@interface SBDateTimeController : NSObject

+ (SBDateTimeController *)sharedInstance;

@property (nonatomic, copy, readonly) NSDate *currentDate;
@property (nonatomic, copy) NSDate *overrideDate;

- (void)addObserver:(id)observer;
- (void)removeObserver:(id)observer;

@end