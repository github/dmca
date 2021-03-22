#import <Preferences/PSTableCell.h>
#import <Preferences/PSHeaderFooterView.h>

/**
 * The `HBImageTableCell` class in `CepheiPrefs` provides a simple way to
 * display an image as a table cell, or a header or footer.
 *
 * ### Specifier Parameters
 * <table>
 * <tr>
 * <th>icon</th> <td>Required. The file name of the image to display in the
 * cell.</td>
 * </tr>
 * </table>
 *
 * If you use `HBImageTableCell` as a header or footer with `headerCellClass`
 * or `footerCellClass`, it will size automatically to fit the image. If you
 * use it as a cell with `cellClass`, you must set the height yourself using
 * the `height` key.
 *
 * ### Example Usage
 * 	<!-- As a header (or footer): -->
 * 	<dict>
 * 		<key>cell</key>
 * 		<string>PSGroupCell</string>
 * 		<key>headerCellClass</key>
 * 		<string>HBImageTableCell</string>
 * 		<key>height</key>
 * 		<integer>100</integer>
 * 		<key>icon</key>
 * 		<string>logo.png</string>
 * 	</dict>
 *
 * 	<!-- As a cell: -->
 * 	<dict>
 * 		<key>cellClass</key>
 * 		<string>HBImageTableCell</string>
 * 		<key>height</key>
 * 		<integer>100</integer>
 * 		<key>icon</key>
 * 		<string>logo.png</string>
 * 	</dict>
 */

@interface HBImageTableCell : PSTableCell <PSHeaderFooterView>

@end
