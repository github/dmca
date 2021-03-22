NS_ASSUME_NONNULL_BEGIN

/**
 * NSString (HBAdditions) is a class category in `Cephei` that provides some convenience methods.
 */
@interface NSString (HBAdditions)

/**
 * Returns a string encoded for an HTTP query parameter.
 *
 * This method encodes a variety of symbols that can conflict with other portions of a URL, such as
 * `&` and `=`, and other similar symbols that could otherwise be misinterpreted by some
 * implementations.
 *
 * @returns A string encoded for an HTTP query parameter.
 */
- (NSString *)hb_stringByEncodingQueryPercentEscapes;

/**
 * Returns a string decoded from an HTTP query parameter.
 *
 * This method decodes percent escapes, as well as spaces encoded with a `+`.
 *
 * @returns A string decoded from an HTTP query parameter.
 */
- (NSString *)hb_stringByDecodingQueryPercentEscapes;

/**
 * Returns a dictionary containing the HTTP query parameters in the string.
 *
 * The string is expected to be in the format `key=value&key=value`, with both keys and values
 * encoded where necessary.
 *
 * @returns An NSDictionary object containing the keys and values from the query string.
 */
- (NSDictionary *)hb_queryStringComponents;

@end

NS_ASSUME_NONNULL_END
