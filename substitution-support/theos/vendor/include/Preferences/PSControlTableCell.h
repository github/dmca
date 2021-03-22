#import "PSTableCell.h"

@interface PSControlTableCell : PSTableCell

- (void)controlChanged:(UIControl *)control;

@property (nonatomic, retain) UIControl *control;
@property (nonatomic, retain) id value;

@end
