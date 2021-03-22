@class SBControlColorSettings;

@interface SBNotificationControlColorSettings : NSObject

- (instancetype)initWithVibrantSettings:(SBControlColorSettings *)vibrantSettings overlaySettings:(SBControlColorSettings *)overlaySettings;

@property (nonatomic, retain, readonly) SBControlColorSettings *vibrantSettings;
@property (nonatomic, retain, readonly) SBControlColorSettings *overlaySettings;

@end
