typedef NS_ENUM(NSInteger, MIBundleType) {
	MIBundleTypeSystemApp = 1,
	MIBundleTypeInternalApp,
	MIBundleTypeDiskImageApp,
	MIBundleTypeUserApp,
	MIBundleTypeVPNPlugin,
	MIBundleTypeCarrierBundle,
	MIBundleTypeAppExtension,
	MIBundleTypeFramework
};

@interface MIBundle : NSObject

+ (BOOL)bundleIsBlacklisted:(MIBundle *)bundle;

+ (instancetype)bundleForURL:(NSURL *)url error:(NSError *)arg2;
+ (instancetype)bundleWithExtension:(NSString *)directory withExtension:(NSString *)extension error:(NSError *)error;

- (instancetype)initWithBundleInDirectory:(NSString *)directory withExtension:(NSString *)extension error:(NSError *)error;
- (instancetype)initWithBundleURL:(NSURL *)url error:(NSError *)error;

- (BOOL)isApplicableToCurrentDeviceCapabilitiesWithError:(NSError *)error;
- (BOOL)isApplicableToCurrentDeviceFamilyWithError:(NSError *)error;
- (BOOL)isApplicableToCurrentOSVersionWithError:(NSError *)error;
- (BOOL)isApplicableToOSVersion:(id)arg1 error:(NSError *)error;
- (BOOL)isCompatibleWithDeviceFamily:(int)deviceFamily; // TODO
- (BOOL)thinningMatchesCurrentDeviceWithError:(NSError *)error;

- (NSArray *)frameworkBundlesWithError:(NSError *)error;
- (NSArray *)pluginKitBundlesWithError:(NSError *)error;

- (BOOL)_scanForBundleInDirectory:(NSString *)directory withExtension:(NSString *)extension error:(NSError *)error;
- (BOOL)_validateWithError:(NSError *)error;

@property (readonly, copy) NSString *identifier;
@property (readonly) NSURL *bundleURL;
@property (retain) NSURL *bundleParentDirectoryURL;
@property (readonly, copy) NSString *bundleName;
@property (readonly) NSDictionary *infoPlistSubset;
@property (readonly, copy) NSString *minimumOSVersion;

@property (readonly) MIBundleType bundleType;
@property (readonly, copy) NSString *bundleTypeDescription;
@property (readonly) BOOL needsDataContainer;

@property (readonly) NSArray *deviceFamilies;
@property (readonly) NSArray *supportedDevices;

@end
