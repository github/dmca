@class SBApplication, SBActivationSettings;

@interface SpringBoard : UIApplication

- (void)applicationOpenURL:(NSURL *)url publicURLsOnly:(BOOL)publicOnly;

- (void)applicationOpenURL:(NSURL *)url publicURLsOnly:(BOOL)publicURLsOnly animating:(BOOL)animating sender:(NSString *)sender additionalActivationFlag:(unsigned)flag NS_DEPRECATED_IOS(5_0, 6_0);
- (void)applicationOpenURL:(NSURL *)url withApplication:(SBApplication *)application sender:(NSString *)sender publicURLsOnly:(BOOL)publicURLsOnly animating:(BOOL)animating needsPermission:(BOOL)needsPermission additionalActivationFlags:(id)flags NS_DEPRECATED_IOS(6_0, 7_0);
- (void)applicationOpenURL:(NSURL *)url withApplication:(SBApplication *)application sender:(NSString *)sender publicURLsOnly:(BOOL)publicURLsOnly animating:(BOOL)animating needsPermission:(BOOL)needsPermission additionalActivationFlags:(id)flags activationHandler:(id)handler NS_DEPRECATED_IOS(7_0, 7_1);
- (void)applicationOpenURL:(NSURL *)url withApplication:(SBApplication *)application sender:(NSString *)sender publicURLsOnly:(BOOL)publicURLsOnly animating:(BOOL)animating needsPermission:(BOOL)needsPermission activationContext:(id)context activationHandler:(id)handler NS_DEPRECATED_IOS(7_1, 8_0);
- (void)applicationOpenURL:(NSURL *)url withApplication:(SBApplication *)application sender:(NSString *)sender publicURLsOnly:(BOOL)publicURLsOnly animating:(BOOL)animating needsPermission:(BOOL)needsPermission activationSettings:(SBActivationSettings *)activationSettings withResult:(id)result NS_DEPRECATED_IOS(8_0, 10_0);
- (void)applicationOpenURL:(NSURL *)url withApplication:(SBApplication *)application publicURLsOnly:(BOOL)publicURLsOnly animating:(BOOL)animating needsPermission:(BOOL)needsPermission activationSettings:(SBActivationSettings *)activationSettings withResult:(id)result NS_DEPRECATED_IOS(10_0, 11_0);
- (void)applicationOpenURL:(NSURL *)url withApplication:(SBApplication *)application animating:(BOOL)animating activationSettings:(SBActivationSettings *)activationSettings origin:(id)origin withResult:(id)result NS_AVAILABLE_IOS(11_0);

- (void)relaunchSpringBoard;
- (void)_relaunchSpringBoardNow;
- (void)undim;

- (void)_simulateLockButtonPress;
- (void)_simulateHomeButtonPress;

- (BOOL)launchApplicationWithIdentifier:(NSString *)identifier suspended:(BOOL)suspended;

- (BOOL)isShowingHomescreen;

- (UIInterfaceOrientation)activeInterfaceOrientation;

@property (nonatomic, retain) UIWindow *window;
@property (readonly) BOOL isLocked;
@property (nonatomic, retain, readonly) SBApplication *_accessibilityFrontMostApplication;

@end
