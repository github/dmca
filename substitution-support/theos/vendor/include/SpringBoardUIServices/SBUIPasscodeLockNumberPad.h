@interface SBUIPasscodeLockNumberPad : NSObject

@property (nonatomic, readonly) NSArray *buttons;

+ (instancetype)_buttonForCharacter:(unsigned)character withLightStyle:(BOOL)lightStyle;

@end
