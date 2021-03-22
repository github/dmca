@class PKGlyphView;

@protocol PKGlyphViewDelegate <NSObject>
@optional

- (void)glyphView:(PKGlyphView *)view revealingCheckmark:(BOOL)revealing;
- (void)glyphView:(PKGlyphView *)view transitioningToState:(NSInteger)state;

@end
