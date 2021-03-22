@class LSApplicationProxy, BSCFBundle;

@interface FBBundleInfo : NSObject

@property (getter=_bundle, nonatomic, readonly, retain) BSCFBundle *bundle;
@property (nonatomic, retain) NSURL *bundleURL;
@property (getter=_proxy, nonatomic, readonly, retain) LSApplicationProxy *proxy;

@property (nonatomic, copy) NSString *bundleIdentifier;
@property (nonatomic, copy) NSString *bundleType;
@property (nonatomic, copy) NSString *bundleVersion;

@property (nonatomic, retain) NSUUID *cacheGUID;

@property (nonatomic, copy) NSString *displayName;
@property (nonatomic, copy) NSDictionary *extendedInfo;
@property (nonatomic) NSUInteger sequenceNumber;

- (instancetype)_initWithApplicationProxy:(LSApplicationProxy *)proxy;
- (instancetype)_initWithBundleURL:(NSURL *)url;

- (void)_purgeBundle;

- (id)extendedInfoValueForKey:(NSString *)key;

@end
