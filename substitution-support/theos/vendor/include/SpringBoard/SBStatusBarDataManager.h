typedef struct {
    char timeString[64];
} SBStatusBarData;

@interface SBStatusBarDataManager : NSObject

+ (instancetype)sharedDataManager;

- (void)_updateTimeString;
- (void)_dataChanged;

@end
