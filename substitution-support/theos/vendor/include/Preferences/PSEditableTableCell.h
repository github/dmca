#import "PSTableCell.h"

@interface PSEditableTableCell : PSTableCell {
	id _userInfo;
	SEL _targetSetter;
	id _realTarget;
}

- (void)controlChanged:(id)changed;
- (void)setValueChangedOnReturn;
- (void)setValueChangedTarget:(id)target action:(SEL)action userInfo:(id)info;

@end
