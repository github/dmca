#import "HBLinkTableCell.h"

/**
 * The `HBPackageTableCell` class in `CepheiPrefs` provides a cell containing
 * any package's icon, name, and description. Tapping it opens the package in
 * in Cydia.
 *
 * ### Specifier Parameters
 * <table>
 * <tr>
 * <th>packageIdentifier</th> <td>Required. The package identifier to retrieve
 * the required information from.</td>
 * </tr>
 * <tr>
 * <th>packageRepository</th> <td>Optional. The URL to the repository the
 * package is available on, if not one of the default repos.</td>
 * </tr>
 * <tr>
 * <th>label</th> <td>Required. The name of the package.</td>
 * </tr>
 * <tr>
 * <th>subtitleText</th> <td>Optional. Can be used for a description of the
 * package.</td>
 * </tr>
 * </tr>
 * </table>
 *
 * ### Example Usage
 * 	<!-- Typical: -->
 * 	<dict>
 * 		<key>cellClass</key>
 * 		<string>HBPackageTableCell</string>
 * 		<key>label</key>
 * 		<string>Cephei</string>
 * 		<key>packageIdentifier</key>
 * 		<string>ws.hbang.common</string>
 * 	</dict>
 *
 * 	<!-- With subtitle: -->
 * 	<dict>
 * 		<key>cellClass</key>
 * 		<string>HBPackageTableCell</string>
 * 		<key>label</key>
 * 		<string>Cephei</string>
 * 		<key>packageIdentifier</key>
 * 		<string>ws.hbang.common</string>
 * 		<key>subtitleText</key>
 * 		<string>Support library for tweaks</string>
 * 	</dict>
 *
 * 	<!-- From a repository: -->
 * 	<dict>
 * 		<key>cellClass</key>
 * 		<string>HBPackageTableCell</string>
 * 		<key>label</key>
 * 		<string>Cephei</string>
 * 		<key>packageIdentifier</key>
 * 		<string>ws.hbang.common</string>
 * 		<key>packageRepository</key>
 * 		<string>https://cydia.hbang.ws</string>
 * 	</dict>
 */
@interface HBPackageTableCell : HBLinkTableCell

@end
