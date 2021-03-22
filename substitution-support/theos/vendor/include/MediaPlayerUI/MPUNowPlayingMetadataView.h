@interface MPUNowPlayingMetadataView : UIView

@property (nonatomic, copy) NSAttributedString *attributedText;

@property (nonatomic) NSUInteger numberOfLines;
@property (nonatomic) BOOL marqueeEnabled;

@end
