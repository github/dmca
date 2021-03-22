@class TPRevealingRingView;

@interface SBPasscodeNumberPadButton : UIView

@property (nonatomic, readonly) TPRevealingRingView *revealingRingView;

@property (retain) CALayer *glyphLayer;

@property (retain) CALayer *highlightedGlyphLayer;

@property unsigned int character;

- (instancetype)initForCharacter:(unsigned int)character;

- (NSString *)stringCharacter;

+ (UIImage *)imageForCharacter:(unsigned)character highlighted:(BOOL)highlighted whiteVersion:(BOOL)whiteVersion;

@end
