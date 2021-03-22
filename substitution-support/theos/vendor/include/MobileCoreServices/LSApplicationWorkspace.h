@interface LSApplicationWorkspace : NSObject

+ (instancetype)defaultWorkspace;

- (void)openApplicationWithBundleID:(NSString *)bundleID;

- (NSArray *)applicationsAvailableForHandlingURLScheme:(NSString *)urlScheme;
- (NSArray *)applicationsAvailableForOpeningURL:(NSURL *)url;
- (NSArray *)applicationsAvailableForOpeningURL:(NSURL *)url legacySPI:(BOOL)legacySPI;

- (NSURL *)URLOverrideForURL:(NSURL *)url;

- (BOOL)registerApplication:(NSURL *)url;
- (BOOL)registerPlugin:(NSURL *)url;

- (BOOL)unregisterApplication:(NSURL *)url;
- (BOOL)unregisterPlugin:(NSURL *)url;

- (NSArray *)pluginsWithIdentifiers:(NSArray *)identifiers protocols:(NSArray *)protocols version:(id)version;

@end
