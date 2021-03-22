@interface SBUserAgent : NSObject

+ (instancetype)sharedUserAgent;

- (void)openURL:(NSURL *)url allowUnlock:(BOOL)allowUnlock animated:(BOOL)animated;

@property (nonatomic, retain) NSString *foregroundApplicationDisplayID;

@end
