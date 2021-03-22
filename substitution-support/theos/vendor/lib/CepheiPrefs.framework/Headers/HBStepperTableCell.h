#import <Preferences/PSControlTableCell.h>

/**
 * The `HBStepperTableCell` class in `CepheiPrefs` allows setting a value using
 * a stepper control ("minus" and "plus" buttons).
 *
 * Requires iOS 6.0 or later.
 *
 * ### Specifier Parameters
 * <table>
 * <tr>
 * <th>label</th> <td>Required. The label displayed when the value is plural.
 * Use <code>%i</code> to denote where the number should be displayed.</td>
 * </tr>
 * <tr>
 * <th>max</th> <td>Required. The highest possible numeric value for the
 * stepper.</td>
 * </tr>
 * <tr>
 * <th>min</th> <td>Required. The lowest possible numeric value for the
 * stepper.</td>
 * </tr>
 * <tr>
 * <th>singularLabel</th> <td>Required. The label displayed when the value is
 * singular.</td>
 * </tr>
 * </table>
 *
 * ### Example Usage
 *	<dict>
 *		<key>cellClass</key>
 *		<string>HBStepperTableCell</string>
 *		<key>default</key>
 *		<real>5</real>
 *		<key>defaults</key>
 *		<string>ws.hbang.common.demo</string>
 *		<key>key</key>
 *		<string>Stepper</string>
 *		<key>label</key>
 *		<string>%i Things</string>
 *		<key>max</key>
 *		<real>15</real>
 *		<key>min</key>
 *		<real>1</real>
 *		<key>singularLabel</key>
 *		<string>1 Thing</string>
 *	</dict>
 */

@interface HBStepperTableCell : PSControlTableCell

@property (nonatomic, retain) UIStepper *control;

@end
