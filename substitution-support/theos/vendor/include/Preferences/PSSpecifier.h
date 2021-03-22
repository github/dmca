#import "PSTableCell.h"
#include <sys/cdefs.h>

__BEGIN_DECLS

extern NSString *const PSAccessoryKey; // @"accessory"
extern NSString *const PSActionKey; // @"action"
extern NSString *const PSAdjustFontSizeToFitWidthKey; // @"adjustFontSizeToFitWidth"
extern NSString *const PSAlignmentKey; // @"alignment"
extern NSString *const PSAppGroupBundleIDKey; // @"appGroupBundleID"
extern NSString *const PSAutoCapsKey; // @"autoCaps"
extern NSString *const PSAutoCorrectionKey; // @"autoCorrection"
extern NSString *const PSBestGuesserKey; // @"bestGuess"
extern NSString *const PSBundleCustomIconPathKey; // @"icon2"
extern NSString *const PSBundleHasBundleIconKey; // @"hasBundleIcon"
extern NSString *const PSBundleHasIconKey; // @"hasIcon"
extern NSString *const PSBundleIconPathKey; // @"icon"
extern NSString *const PSBundleIsControllerKey; // @"isController"
extern NSString *const PSBundleOverridePrincipalClassKey; // @"overridePrincipalClass"
extern NSString *const PSBundlePathKey; // @"bundle"
extern NSString *const PSBundleTintedIconPathKey; // @"tintedIcon"
extern NSString *const PSButtonActionKey; // @"buttonAction"
extern NSString *const PSCancelKey; // @"cancel"
extern NSString *const PSCellClassKey; // @"cellClass"
extern NSString *const PSConfirmationActionKey; // @"confirmationAction"
extern NSString *const PSConfirmationCancelActionKey; // @"cancel"
extern NSString *const PSConfirmationCancelKey; // @"cancelTitle"
extern NSString *const PSConfirmationDestructiveKey; // @"isDestructive"
extern NSString *const PSConfirmationKey; // @"confirmation"
extern NSString *const PSConfirmationOKKey; // @"okTitle"
extern NSString *const PSConfirmationPromptKey; // @"prompt"
extern NSString *const PSConfirmationTitleKey; // @"title"
extern NSString *const PSContainerBundleIDKey; // @"containerBundleID"
extern NSString *const PSControlIsLoadingKey; // @"control-loading"
extern NSString *const PSControlKey; // @"control"
extern NSString *const PSControllerLoadActionKey; // @"loadAction"
extern NSString *const PSControlMaximumKey; // @"max"
extern NSString *const PSControlMinimumKey; // @"min"
extern NSString *const PSCopyableCellKey; // @"isCopyable"
extern NSString *const PSDataSourceClassKey; // @"dataSourceClass"
extern NSString *const PSDecimalKeyboardKey; // @"isDecimalPad"
extern NSString *const PSDefaultsKey; // @"defaults"
extern NSString *const PSDefaultValueKey; // @"default"
extern NSString *const PSDeferItemSelectionKey; // @"deferItemSelection"
extern NSString *const PSDeletionActionKey; // @"deletionAction"
extern NSString *const PSDetailControllerClassKey; // @"detail"
extern NSString *const PSEditableTableCellTextFieldShouldPopOnReturn; // @"textFieldShouldPopOnReturn"
extern NSString *const PSEditPaneClassKey; // @"pane"
extern NSString *const PSEmailAddressingKeyboardKey; // @"isEmailAddressing"
extern NSString *const PSEmailAddressKeyboardKey; // @"isEmail"
extern NSString *const PSEnabledKey; // @"enabled"
extern NSString *const PSFooterAlignmentGroupKey; // @"footerAlignment"
extern NSString *const PSFooterCellClassGroupKey; // @"footerCellClass"
extern NSString *const PSFooterTextGroupKey; // @"footerText"
extern NSString *const PSFooterViewKey; // @"footerView"
extern NSString *const PSGetterKey; // @"get"
extern NSString *const PSHeaderCellClassGroupKey; // @"headerCellClass"
extern NSString *const PSHeaderDetailTextGroupKey; // @"headerDetailText"
extern NSString *const PSHeaderViewKey; // @"headerView"
extern NSString *const PSHidesDisclosureIndicatorKey; // @"hidesDisclosureIndicator"
extern NSString *const PSIconImageKey; // @"iconImage"
extern NSString *const PSIconImageShouldFlipForRightToLeftKey; // @"iconImageShouldFlipForRightToLeft"
extern NSString *const PSIDKey; // @"id"
extern NSString *const PSIPKeyboardKey; // @"isIP"
extern NSString *const PSIsPerGizmoKey; // @"isPerGizmo"
extern NSString *const PSIsRadioGroupKey; // @"isRadioGroup"
extern NSString *const PSIsTopLevelKey; // @"isTopLevel"
extern NSString *const PSKeyboardTypeKey; // @"keyboard"
extern NSString *const PSKeyNameKey; // @"key"
extern NSString *const PSLazilyLoadedBundleKey; // @"lazy-bundle"
extern NSString *const PSLazyIconAppID; // @"appIDForLazyIcon"
extern NSString *const PSLazyIconDontUnload; // @"dontUnloadLazyIcon"
extern NSString *const PSLazyIconLoading; // @"useLazyIcons"
extern NSString *const PSLazyIconLoadingCustomQueue; // @"customIconQueue"
extern NSString *const PSMarginWidthKey; // @"marginWidth"
extern NSString *const PSNegateValueKey; // @"negate"
extern NSString *const PSNotifyNanoKey; // @"notifyNano"
extern NSString *const PSNumberKeyboardKey; // @"isNumeric"
extern NSString *const PSPlaceholderKey; // @"placeholder"
extern NSString *const PSPrioritizeValueTextDisplayKey; // @"prioritizeValueTextDisplay"
extern NSString *const PSRadioGroupCheckedSpecifierKey; // @"radioGroupCheckedSpecifier"
extern NSString *const PSRequiredCapabilitiesKey; // @"requiredCapabilities"
extern NSString *const PSRequiredCapabilitiesOrKey; // @"requiredCapabilitiesOr"
extern NSString *const PSSearchNanoApplicationsBundlePath; // @"nanoAppsBundlePath"
extern NSString *const PSSearchNanoInternalSettingsBundlePath; // @"nanoInternalBundlePath"
extern NSString *const PSSearchNanoSettingsBundlePath; // @"nanoBundlePath"
extern NSString *const PSSetterKey; // @"set"
extern NSString *const PSSetupCustomClassKey; // @"customControllerClass"
extern NSString *const PSShortTitlesDataSourceKey; // @"shortTitlesDataSource"
extern NSString *const PSShortTitlesKey; // @"shortTitles"
extern NSString *const PSSliderIsContinuous; // @"isContinuous"
extern NSString *const PSSliderIsSegmented; // @"isSegmented"
extern NSString *const PSSliderLeftImageKey; // @"leftImage"
extern NSString *const PSSliderLeftImagePromiseKey; // @"leftImagePromise"
extern NSString *const PSSliderRightImageKey; // @"rightImage"
extern NSString *const PSSliderRightImagePromiseKey; // @"rightImagePromise"
extern NSString *const PSSliderSegmentCount; // @"segmentCount"
extern NSString *const PSSliderShowValueKey; // @"showValue"
extern NSString *const PSSpecifierAuthenticationTokenKey; // @"authenticationToken"
extern NSString *const PSSpecifierPasscodeKey; // @"passcode"
extern NSString *const PSStaticTextMessageKey; // @"staticTextMessage"
extern NSString *const PSTableCellClassKey; // @"cell"
extern NSString *const PSTableCellHeightKey; // @"height"
extern NSString *const PSTableCellKey; // @"cellObject"
extern NSString *const PSTableCellUseEtchedAppearanceKey; // @"useEtched"
extern NSString *const PSTextFieldNoAutoCorrectKey; // @"noAutoCorrect"
extern NSString *const PSTextViewBottomMarginKey; // @"textViewBottomMargin"
extern NSString *const PSTitleKey; // @"label"
extern NSString *const PSTitlesDataSourceKey; // @"titlesDataSource"
extern NSString *const PSURLKeyboardKey; // @"isURL"
extern NSString *const PSValidTitlesKey; // @"validTitles"
extern NSString *const PSValidValuesKey; // @"validValues"
extern NSString *const PSValueChangedNotificationKey; // @"PostNotification"
extern NSString *const PSValueKey; // @"value"
extern NSString *const PSValuesDataSourceKey; // @"valuesDataSource"

__END_DECLS

@interface PSSpecifier : NSObject {
@public
	SEL action;
}

+ (instancetype)preferenceSpecifierNamed:(NSString *)identifier target:(id)target set:(SEL)set get:(SEL)get detail:(Class)detail cell:(PSCellType)cellType edit:(Class)edit;
+ (instancetype)emptyGroupSpecifier;
+ (instancetype)groupSpecifierWithName:(NSString *)name;

@property (nonatomic, retain) id target;
@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *identifier;

@property (nonatomic) PSCellType cellType;

@property (nonatomic) Class detailControllerClass;

@property (nonatomic) SEL buttonAction;
@property (nonatomic) SEL confirmationAction;
@property (nonatomic) SEL confirmationCancelAction;
@property (nonatomic) SEL controllerLoadAction;

@property (nonatomic, retain) NSMutableDictionary *properties;

- (id)propertyForKey:(NSString *)key;
- (void)setProperty:(id)property forKey:(NSString *)key;
- (void)removePropertyForKey:(NSString *)key;

@property (nonatomic, retain) NSDictionary *shortTitleDictionary;
@property (nonatomic, retain) NSDictionary *titleDictionary;

@end
