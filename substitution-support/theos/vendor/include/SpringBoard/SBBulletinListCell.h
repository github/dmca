typedef enum {
	SBBulletinListCellAccessoryStyleNone = 0,
	SBBulletinListCellAccessoryStyleDot = 1
} SBBulletinListCellAccessoryStyle;

@interface SBBulletinListCell : UITableViewCell

@property SBBulletinListCellAccessoryStyle bulletinAccessoryStyle;

- (instancetype)initWithLinenView:(UIView *)linenView reuseIdentifier:(NSString *)reuseIdentifier;

@end
