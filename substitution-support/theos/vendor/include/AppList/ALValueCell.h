#import <UIKit/UIKit.h>

@protocol ALValueCellDelegate;

@interface ALValueCell : UITableViewCell {
@private
	id<ALValueCellDelegate> delegate;
}

@property (nonatomic, assign) id<ALValueCellDelegate> delegate;

- (void)loadValue:(id)value; // Deprecated
- (void)loadValue:(id)value withTitle:(NSString *)title;

- (void)didSelect;

@end

@protocol ALValueCellDelegate <NSObject>
@required
- (void)valueCell:(ALValueCell *)valueCell didChangeToValue:(id)newValue;
@end

@interface ALSwitchCell : ALValueCell {
@private
	UISwitch *switchView;
}

@property (nonatomic, readonly) UISwitch *switchView;

@end

@interface ALCheckCell : ALValueCell

@end

@interface ALDisclosureIndicatedCell : ALValueCell

@end

