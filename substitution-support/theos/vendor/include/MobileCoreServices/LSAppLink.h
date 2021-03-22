@class LSApplicationProxy;

typedef NS_ENUM(NSInteger, LSAppLinkOpenStrategy) {
	LSAppLinkOpenStrategyUnknown = -1,
	LSAppLinkOpenStrategyApp,
	LSAppLinkOpenStrategyIdk,
	LSAppLinkOpenStrategyBrowser,
};

@interface LSAppLink : NSObject

+ (instancetype)_appLinkWithURL:(NSURL *)url applicationProxy:(LSApplicationProxy *)applicationProxy plugIn:(id)plugIn error:(NSError **)error NS_DEPRECATED_IOS(9_0, 11_0);
+ (instancetype)_appLinkWithURL:(NSURL *)url applicationProxy:(LSApplicationProxy *)applicationProxy plugIn:(id)plugIn NS_AVAILABLE_IOS(11_0);

@property (nonatomic, retain) NSURL *URL;
@property (nonatomic, retain) LSApplicationProxy *targetApplicationProxy;
@property LSAppLinkOpenStrategy openStrategy;

@end
