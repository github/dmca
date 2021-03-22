@interface CKTranscriptRecipientsHeaderFooterView : UITableViewHeaderFooterView

+ (NSString *)identifier;

- (instancetype)initWithReuseIdentifier:(NSString *)reuseIdentifier;

@property (nonatomic, retain) UILabel *preceedingSectionFooterLabel;
@property (nonatomic, retain) UIView *bottomSeparator;

@property CGFloat margin;

@end
