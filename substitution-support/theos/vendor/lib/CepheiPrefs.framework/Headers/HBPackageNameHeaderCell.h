#import <Preferences/PSTableCell.h>
#import <Preferences/PSHeaderFooterView.h>

/**
 * The `HBPackageNameHeaderCell` class in `CepheiPrefs` displays a header
 * containing the package's icon, name, version number, and author. It can be
 * displayed in a subtle condensed design, or, by default, a tall header that
 * might be displayed at the top of a preference bundle's root list controller,
 * for instance.
 *
 * ### Specifier Parameters
 * <table>
 * <tr>
 * <th>condensed</th> <td>Optional. When true, displays an icon, the package
 * name and version in one line, and on another displays the author name.
 * When false, displays a large package name, and on two lines in small font
 * the package version and author. The default is false.</td>
 * </tr>
 * <tr>
 * <th>icon</th> <td>Required in condensed mode. Not used otherwise. The file
 * name of the icon to use within the current preference bundle.</td>
 * </tr>
 * <tr>
 * <th>packageIdentifier</th> <td>Required. The package identifier to retrieve
 * the required information from.</td>
 * </tr>
 * <tr>
 * <th>packageNameOverride</th> <td>Optional. A custom name to use instead of
 * the package’s name.</td>
 * </tr>
 * <tr>
 * <th>showAuthor</th> <td>Optional. Whether to show the `Author` field of the
 * package. The default is true.</td>
 * </tr>
 * <tr>
 * <th>showVersion</th> <td>Optional. Whether to show the `Version` field of
 * the package. The default is true.</td>
 * </tr>
 * <tr>
 * <th>titleColor</th> <td>Optional. The color to apply to the name of the
 * package. The default is #111111.</td>
 * </tr>
 * <tr>
 * <th>subtitleColor</th> <td>Optional. The color to apply to the subtitles.
 * The default is #444444.</td>
 * </tr>
 * <tr>
 * <th>backgroundGradientColors</th> <td>Optional. An array of color stops to
 * use as a background gradient. At least one is required. The default is no
 * background gradient.</td>
 * </tr>
 * </table>
 *
 * ### Example Usage
 * 	<!-- Standard size: -->
 * 	<dict>
 * 		<key>cell</key>
 * 		<string>PSGroupCell</string>
 * 		<key>headerCellClass</key>
 * 		<string>HBPackageNameHeaderCell</string>
 * 		<key>packageIdentifier</key>
 * 		<string>ws.hbang.common</string>
 * 	</dict>
 *
 * 	<!-- Condensed size: -->
 * 	<dict>
 * 		<key>cell</key>
 * 		<string>PSGroupCell</string>
 * 		<key>condensed</key>
 * 		<true/>
 * 		<key>headerCellClass</key>
 * 		<string>HBPackageNameHeaderCell</string>
 * 		<key>icon</key>
 * 		<string>icon.png</string>
 * 		<key>packageIdentifier</key>
 * 		<string>ws.hbang.common</string>
 * 	</dict>
 *
 * 	<!-- Standard size with custom colors: -->
 * 	<dict>
 * 		<key>cell</key>
 * 		<string>PSGroupCell</string>
 * 		<key>headerCellClass</key>
 * 		<string>HBPackageNameHeaderCell</string>
 * 		<key>packageIdentifier</key>
 * 		<string>ws.hbang.common</string>
 * 		<key>titleColor</key>
 * 		<string>#CC0000</string>
 * 		<key>subtitleColor</key>
 * 		<array>
 * 			<integer>55</integer>
 * 			<integer>147</integer>
 * 			<integer>230</integer>
 * 		</array>
 * 	</dict>
 *
 * 	<!-- Standard size with gradient background: -->
 * 	<dict>
 * 		<key>cell</key>
 * 		<string>PSGroupCell</string>
 * 		<key>headerCellClass</key>
 * 		<string>HBPackageNameHeaderCell</string>
 * 		<key>packageIdentifier</key>
 * 		<string>ws.hbang.common</string>
 * 		<key>backgroundGradientColors</key>
 * 		<array>
 * 			<string>#5AD427</string>
 * 			<string>#FFDB4C</string>
 * 			<string>#EF4DB6</string>
 * 			<string>#898C90</string>
 * 		</array>
 * 	</dict>
 */

@interface HBPackageNameHeaderCell : PSTableCell <PSHeaderFooterView>

@end
