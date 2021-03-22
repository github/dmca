typedef NS_ENUM(NSUInteger, ABMonogrammerStyle) {
	ABMonogrammerStyleLightGray,
	ABMonogrammerStyleWhite,
	ABMonogrammerStyleDarkGraySemitransparent,
	ABMonogrammerStyleLightGraySemitransparent,
};

@interface ABMonogrammer : NSObject

- (instancetype)initWithStyle:(ABMonogrammerStyle)style diameter:(CGFloat)diameter;

- (UIImage *)_copyMonogramWithKnockoutMask;

- (void)monogramsAsFlatImages;

@end
