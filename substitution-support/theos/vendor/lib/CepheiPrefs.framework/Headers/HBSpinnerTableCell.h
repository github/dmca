#import "HBTintedTableCell.h"

/**
 * The `HBSpinnerTableCell` class in `CepheiPrefs` displays an activity
 * indicator when the cell is disabled.
 *
 * ### Example Usage
 * Specifier plist:
 *
 * 	<dict>
 *		<key>action</key>
 *		<string>doStuffTapped:</string>
 * 		<key>cell</key>
 * 		<string>PSButtonCell</string>
 * 		<key>cellClass</key>
 * 		<string>HBSpinnerTableCell</string>
 * 		<key>label</key>
 * 		<string>Do Stuff</string>
 * 	</dict>
 *
 * List controller implementation:
 *
 *	- (void)doStuffTapped:(PSSpecifier *)specifier {
 *		PSTableCell *cell = [self cachedCellForSpecifier:specifier];
 * 		cell.cellEnabled = NO;
 * 		// do something in the background...
 * 		cell.cellEnabled = YES;
 *	}
 */

@interface HBSpinnerTableCell : HBTintedTableCell

@end
