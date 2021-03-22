NS_ASSUME_NONNULL_BEGIN

/**
 * UIColor (HBAdditions) is a class category in `Cephei` that provides some
 * convenience methods.
 */
@interface UIColor (HBAdditions)

/**
 * Creates and returns a color object using data from the specified object.
 *
 * The value is expected to either be an array of 3 or 4 integer RGB or RGBA
 * color components (respectively), with values between 0 and 255, or a CSS
 * hexadecimal color code string.
 *
 * @param value The object to retrieve data from. See the discussion for the
 * supported object types.
 * @returns The color object. The color information represented by this object
 * is in the device RGB colorspace.
 */
+ (instancetype)hb_colorWithPropertyListValue:(id)value;

/**
 * Initializes and returns a color object using data from the specified object.
 *
 * The value is expected to either be an array of 3 or 4 integer RGB or RGBA
 * color components (respectively), with values between 0 and 255, or a CSS
 * hexadecimal color code string.
 *
 * @param value The object to retrieve data from. See the discussion for the
 * supported object types.
 * @returns An initialized color object. The color information represented by
 * this object is in the device RGB colorspace.
 */
- (instancetype)hb_initWithPropertyListValue:(id)value;

@end

NS_ASSUME_NONNULL_END
