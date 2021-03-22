typedef NS_ENUM(NSUInteger, SBPocketState) {
    SBPocketStateOutOfPocket,
    SBPocketStateInPocket,
    SBPocketStateFaceDown,
    SBPocketStateFaceDownOnTable,
    SBPocketStateUnknown
};

@interface SBPocketStateMonitor : NSObject

+ (instancetype)sharedInstance;

@property (readonly, nonatomic) SBPocketState pocketState;

@end