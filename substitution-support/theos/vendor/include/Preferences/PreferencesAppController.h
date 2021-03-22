@class PSUIPrefsRootController;

@interface PreferencesAppController : UIApplication/*this is from Preferences.app, not framework*/

@property (nonatomic, retain) PSUIPrefsRootController *rootController;

- (void)generateURL;

@end
