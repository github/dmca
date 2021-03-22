@class PSSpecifier, UITableView;

@protocol PSHeaderFooterView

@required
- (UIView<PSHeaderFooterView> *)initWithSpecifier:(PSSpecifier *)specifier;

@optional
- (CGFloat)preferredHeightForWidth:(CGFloat)width inTableView:(UITableView *)tableView;
- (CGFloat)preferredHeightForWidth:(CGFloat)width;

@end
