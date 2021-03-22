@interface BBThumbnailSizeConstraints : NSObject

@property (nonatomic) CGFloat fixedWidth;
@property (nonatomic) CGFloat fixedHeight;
@property (nonatomic) CGFloat fixedDimension;

@property (nonatomic) CGFloat minAspectRatio;
@property (nonatomic) CGFloat maxAspectRatio;

@property (nonatomic) CGFloat thumbnailScale;

- (CGSize)sizeFromAspectRatio:(CGFloat)aspectRatio;

@end
