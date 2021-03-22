#import "HBLinkTableCell.h"

/**
 * The `HBTwitterCell` class in `CepheiPrefs` displays a button containing a
 * person's name, along with their Twitter username and avatar. When tapped,
 * a Twitter client installed on the user's device or the Twitter website is
 * opened to the person's profile.
 *
 * ### Specifier Parameters
 * <table>
 * <tr>
 * <th>big</th> <td>Optional. Whether to display the username below the name
 * (true) or to the right of it (false). The default is false. If you set this
 * to true, you should also set the cell's height to 56pt.</td>
 * </tr>
 * <tr>
 * <th>initials</th> <td>Optional. One or two characters to show instead of an
 * avatar.</td>
 * </tr>
 * <tr>
 * <th>label</th> <td>Required. The name of the person.</td>
 * </tr>
 * <tr>
 * <th>showAvatar</th> <td>Optional. Whether to show the avatar of the user.
 * The default is true.</td>
 * </tr>
 * <tr>
 * <th>user</th> <td>Required. The Twitter username of the person.</td>
 * </tr>
 * </table>
 *
 * ### Example Usage
 * 	<!-- Standard size: -->
 * 	<dict>
 * 		<key>cellClass</key>
 * 		<string>HBTwitterCell</string>
 * 		<key>label</key>
 * 		<string>HASHBANG Productions</string>
 * 		<key>user</key>
 * 		<string>hbangws</string>
 * 	</dict>
 *
 * 	<!-- Big size: -->
 * 	<dict>
 * 		<key>big</key>
 * 		<true/>
 * 		<key>cellClass</key>
 * 		<string>HBTwitterCell</string>
 * 		<key>height</key>
 * 		<integer>56</integer>
 * 		<key>label</key>
 * 		<string>HASHBANG Productions</string>
 * 		<key>user</key>
 * 		<string>hbangws</string>
 * 	</dict>
 *
 * 	<!-- Without an avatar: -->
 * 	<dict>
 * 		<key>cellClass</key>
 * 		<string>HBTwitterCell</string>
 * 		<key>label</key>
 * 		<string>HASHBANG Productions</string>
 * 		<key>showAvatar</key>
 * 		<false/>
 * 		<key>user</key>
 * 		<string>hbangws</string>
 * 	</dict>
 */

@interface HBTwitterCell : HBLinkTableCell

@end
