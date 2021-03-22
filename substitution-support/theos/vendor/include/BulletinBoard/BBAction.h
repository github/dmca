typedef void (^BBActionCallblock)();

@class BBAppearance;

@interface BBAction : NSObject

+ (instancetype)action;

+ (instancetype)actionWithLaunchBundleID:(NSString *)bundleID callblock:(BBActionCallblock)callblock;
+ (instancetype)actionWithLaunchURL:(NSURL *)url callblock:(BBActionCallblock)callblock;
+ (instancetype)actionWithCallblock:(BBActionCallblock)callblock;

+ (instancetype)actionWithAppearance:(BBAppearance *)appearance;

@property (nonatomic, copy) NSString *identifier;

@property (nonatomic, retain, readonly) NSString *bundleID;
@property (nonatomic, copy) NSString *launchBundleID;
@property (nonatomic, copy) NSURL *launchURL;

- (void)setCallblock:(BBActionCallblock)callblock;

@end
