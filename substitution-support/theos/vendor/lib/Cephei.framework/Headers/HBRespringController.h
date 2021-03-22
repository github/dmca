NS_ASSUME_NONNULL_BEGIN

/**
 * The `HBRespringController` class in `Cephei` provides conveniences for restarting the system app
 * (usually SpringBoard). It also ensures battery usage statistics are not lost when performing the
 * restart.
 */
@interface HBRespringController : NSObject

/**
 * Restart the system app.
 *
 * On iOS 8.0 and newer, fades out and then returns to the home screen (system remains unlocked). On
 * older iOS versions, a standard restart occurs.
 */
+ (void)respring;

/**
 * Restart the system app and immediately launch a URL.
 *
 * Requires iOS 8.0 or newer. On older iOS versions, a standard restart occurs and the URL is not
 * opened.
 *
 * @param returnURL The URL to launch after restarting.
 */
+ (void)respringAndReturnTo:(nullable NSURL *)returnURL;

@end

NS_ASSUME_NONNULL_END
