@interface ANEMSettingsManager : NSObject

+ (instancetype)sharedManager;

@property (nonatomic, retain, readonly) NSArray <NSString *> *themeSettings;

@end
