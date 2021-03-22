#import <Preferences/PSSliderTableCell.h>
#import <Preferences/PSDiscreteSlider.h>

/**
 * The `HBDiscreteSliderTableCell` class in `CepheiPrefs` is a subclass of the
 * standard slider cell, which displays a vertical line at every whole number.
 * Additionally, when dragging the slider, it jumps to these lines so the user’s
 * preference will always be a whole number.
 *
 * It is no longer necessary to use this as of iOS 8.2, which has built in
 * `isSegmented` and `segmentCount` parameters on PSSliderCell. This class is
 * kept for backwards compatibility, and will use the built-in implementation on
 * iOS 8.2 and newer.
 *
 * Requires iOS 7.0 or later. A normal slider is shown for older versions.
 *
 * ### Specifier Parameters
 * All parameters specific to
 * [PSSliderCell](http://iphonedevwiki.net/index.php/Preferences_specifier_plist#PSSliderCell)
 * are applicable here. There are no custom parameters.
 *
 * ### Example Usage
 *	<dict>
 *		<key>cell</key>
 *		<string>PSSliderCell</string>
 *		<key>cellClass</key>
 *		<string>HBDiscreteSliderTableCell</string>
 *		<key>default</key>
 *		<real>5</real>
 *		<key>defaults</key>
 *		<string>ws.hbang.common.demo</string>
 *		<key>key</key>
 *		<string>Discrete</string>
 *		<key>label</key>
 *		<string>Discrete</string>
 *		<key>max</key>
 *		<real>15</real>
 *		<key>min</key>
 *		<real>1</real>
 *	</dict>
 */

@interface HBDiscreteSliderTableCell : PSControlTableCell

@property (nonatomic, retain) PSDiscreteSlider *control;

@end
