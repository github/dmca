#import "FBBundleInfo.h"

@class FBApplicationDefaults;

@interface FBApplicationInfo : FBBundleInfo

@property (nonatomic, readonly, copy) NSString *applicationIdentifierEntitlement;
@property (getter=isBeta, nonatomic, readonly) BOOL beta;
@property (nonatomic, readonly, retain) NSURL *bundleContainerURL;
@property (nonatomic, readonly, retain) NSArray *customMachServices;
@property (nonatomic, readonly, retain) NSURL *dataContainerURL;
@property (nonatomic, readonly, retain) FBApplicationDefaults *defaults;
@property (nonatomic, readonly, retain) NSArray *deviceFamilies;
@property (nonatomic, readonly, retain) NSNumber *downloaderDSID;
@property (getter=isEnabled, nonatomic, readonly) BOOL enabled;
@property (nonatomic, readonly, retain) NSDictionary *entitlements;
@property (nonatomic, readonly, retain) NSDictionary *environmentVariables;
@property (nonatomic, readonly, retain) NSURL *executableURL;
@property (getter=isExitsOnSuspend, nonatomic, readonly) BOOL exitsOnSuspend;
@property (nonatomic, readonly, retain) NSArray *externalAccessoryProtocols;
@property (nonatomic, readonly, retain) NSString *fallbackFolderName;
@property (nonatomic, readonly, retain) NSArray *folderNames;
@property (getter=hasFreeDeveloperProvisioningProfile, nonatomic, readonly) BOOL freeDeveloperProvisioningProfile;
@property (getter=_isInstalling, setter=_setInstalling:, nonatomic) BOOL installing;
@property (nonatomic, readonly) double lastModifiedDate;
@property (nonatomic, readonly) float minimumBrightnessLevel;
@property (getter=isNewsstand, nonatomic, readonly) BOOL newsstand;
@property (nonatomic, readonly, copy) NSString *preferenceDomain;
@property (getter=isProvisioningProfileValidated, nonatomic, readonly) BOOL provisioningProfileValidated;
@property (nonatomic, readonly, retain) NSNumber *purchaserDSID;
@property (nonatomic, readonly) int ratingRank;
@property (nonatomic, readonly, retain) NSArray *requiredCapabilities;
@property (nonatomic, readonly) BOOL requiresPersistentWiFi;
@property (getter=isRestricted, nonatomic, readonly) BOOL restricted;
@property (nonatomic, readonly, retain) NSURL *sandboxURL;
@property (nonatomic, readonly, copy) NSString *sdkVersion;
@property (nonatomic, readonly) int signatureState;
@property (nonatomic, readonly, copy) NSString *signerIdentity;
@property (nonatomic, readonly) unsigned int supportedInterfaceOrientations;
@property (nonatomic, readonly) unsigned int type;
@property (getter=_isUninstalling, setter=_setUninstalling:, nonatomic) BOOL uninstalling;
@property (getter=hasUniversalProvisioningProfile, nonatomic, readonly) BOOL universalProvisioningProfile;

@end
