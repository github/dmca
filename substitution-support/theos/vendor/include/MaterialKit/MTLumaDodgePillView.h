typedef NS_ENUM(NSUInteger, MTLumaDodgePillStyle) {
	MTLumaDodgePillStyleNone,
	MTLumaDodgePillStyleThin,
	MTLumaDodgePillStyleGray,
	MTLumaDodgePillStyleBlack,
	MTLumaDodgePillStyleWhite
};

typedef NS_ENUM(NSUInteger, MTLumaDodgePillBackgroundLuminance) {
	MTLumaDodgePillBackgroundLuminanceUnknown,
	MTLumaDodgePillBackgroundLuminanceDark,
	MTLumaDodgePillBackgroundLuminanceLight
};

@interface MTLumaDodgePillView : UIView

@property (nonatomic) MTLumaDodgePillStyle style;
@property (nonatomic, readonly) MTLumaDodgePillBackgroundLuminance backgroundLuminance;

+ (CGFloat)suggestedEdgeSpacing;
+ (CGSize)suggestedSizeForContentWidth:(CGFloat)width;

@end
