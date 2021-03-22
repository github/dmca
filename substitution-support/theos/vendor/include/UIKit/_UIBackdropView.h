@class _UIBackdropViewSettings, _UIBackdropEffectView;

/*
_UIBackdropViewSettingsCombiner			-3
_UIBackdropViewSettingsNone				-2
_UIBackdropViewSettingsLight			0,
										1000,
										1003,
										2020,
										10090,
										10100
_UIBackdropViewSettingsDark				1,		iOS 7 Notification Center (?), leopard
										1001,
										1100,
										2030,
										11050,
										11060
_UIBackdropViewSettingsBlur				2		Common blurring
_UIBackdropViewSettingsColorSample		2000
_UIBackdropViewSettingsUltraLight		2010	Some alert views in Settings app & many white UIs
_UIBackdropViewSettingsLightLow			2029
_UIBackdropViewSettingsDarkWithZoom		2031
_UIBackdropViewSettingsDarkLow			2039
_UIBackdropViewSettingsColored			2040	rgba(0.0196078,0.0196078,0.0196078,1)
										10091	rgba(0.160784,1,0.301961,1)
										10092	rgba(1,0.0980392,0.0470588,1)
										10120	rgba(0.0313725,0.262745,0.560784,1)
_UIBackdropViewSettingsUltraDark		2050,
										11070
_UIBackdropViewSettingsAdaptiveLight	2060	iOS 7 Control Center
_UIBackdropViewSettingsSemiLight		2070
_UIBackdropViewSettingsFlatSemiLight	2071
_UIBackdropViewSettingsUltraColored		2080
_UIBackdropViewSettingsPasscodePaddle	3900	iOS 7.1+
_UIBackdropViewSettingsLightLeopard		3901	iOS 7.1+
*/

typedef NS_ENUM(NSUInteger, _UIBackdropViewStyle) {
	_UIBackdropViewStyleNone = -2,
	_UIBackdropViewStyleLight = 0,
	_UIBackdropViewStyleDark,
	_UIBackdropViewStyleBlur
};

@interface _UIBackdropView : UIView

- (instancetype)initWithFrame:(CGRect)frame autosizesToFitSuperview:(BOOL)autosizes settings:(_UIBackdropViewSettings *)settings;

- (void)transitionToSettings:(_UIBackdropViewSettings *)settings;
- (void)transitionToStyle:(_UIBackdropViewStyle)style;
- (void)transitionToPrivateStyle:(NSUInteger)privateStyle; // currently calls the above method

@property (nonatomic, retain) _UIBackdropEffectView *backdropEffectView;

@property (nonatomic, retain) _UIBackdropViewSettings *inputSettings;

@end
