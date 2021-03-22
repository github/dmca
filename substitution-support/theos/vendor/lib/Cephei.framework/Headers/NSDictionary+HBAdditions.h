NS_ASSUME_NONNULL_BEGIN

/**
 * NSDictionary (HBAdditions) is a class category in `Cephei` that provides some convenience methods.
 */
@interface NSDictionary (HBAdditions)

/**
 * Constructs and returns an NSString object that is the result of joining the dictionary keys and
 * values into an HTTP query string.
 *
 * On iOS 8.0 and newer, this uses the built in NSURLComponents functionality to deserialize the
 * query string. On earlier versions, uses an approximation implemented within Cephei. This
 * implementation is simplistic and does not handle edge cases that NSURLComponents does support.
 *
 * @returns An NSString containing an HTTP query string.
 */
- (NSString *)hb_queryString;

@end

NS_ASSUME_NONNULL_END
