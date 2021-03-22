#import "PSControlTableCell.h"

@class UIActivityIndicatorView;

@interface PSSwitchTableCell : PSControlTableCell {
	UIActivityIndicatorView *_activityIndicator;
}

@property (nonatomic) BOOL loading;

@end
