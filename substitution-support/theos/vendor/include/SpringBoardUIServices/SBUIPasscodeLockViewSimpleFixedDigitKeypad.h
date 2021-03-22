@class SBUIPasscodeLockNumberPad;
@class SBPasscodeNumberPadButton;

@interface SBUIPasscodeLockViewSimpleFixedDigitKeypad : UIView

@property (nonatomic) CGFloat backgroundAlpha;

- (instancetype)initWithLightStyle:(BOOL)lightStyle;

- (void)passcodeLockNumberPad:(SBUIPasscodeLockNumberPad *)numberPad keyUp:(SBPasscodeNumberPadButton *)passcodeButton;

- (SBUIPasscodeLockNumberPad *)_numberPad;

@end
