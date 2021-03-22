#import "HBLOHandler.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * The HBLibOpener class provides the ability to manually register a handler
 * with Opener from a tweak, as well as the ability to check if a handler has
 * been disabled by the user.
 */
@interface HBLibOpener : NSObject

/**
 * Returns the shared instance of the HBLibOpener class.
 *
 * @returns The shared instance of the HBLibOpener class.
 */
+ (instancetype)sharedInstance;

/**
 * Allows you to register a handler with libopener.
 *
 * You should call this from all processes that link against the
 * MobileCoreServices framework (`com.apple.MobileCoreServices`). See the
 * [Cydia Substrate documentation](http://www.cydiasubstrate.com/inject/darwin/)
 * for details on filter plists.
 *
 * @param handler A handler to be registered. See HBLOHandler.
 * @param error A pointer to an error object. If an error occurs, this pointer
 * is set to an error object containing the error information. You may specify
 * nil for this parameter if you do not want the error information.
 * @returns YES if the handler was registered successfully. Returns NO if an
 * error occurred.
 */
- (BOOL)registerHandler:(HBLOHandler *)handler error:(NSError **)error;

/**
 * Raises an exception.
 *
 * Supported in SpringBoard only. **Deprecated in Opener 2.0. Removed in Opener
 * 3.0.** Use bundles or registerHandler:error: instead.
 *
 * The old handler API is deprecated. Links in web views are not supported.
 * Migrate to the new API as soon as possible.
 *
 * @param name Unused.
 * @param block Unused.
 * @returns Never returns.
 */
- (BOOL)registerHandlerWithName:(NSString *)name block:(id)block;

/**
 * Returns whether a handler is enabled or disabled.
 *
 * @param handler The identifier of the handler.
 * @returns YES if the handler is enabled, or uses a custom Preferences list
 * controller. Returns NO if the handler has been disabled by the user in the
 * Opener section of Settings.
 */
- (BOOL)handlerIsEnabled:(NSString *)handler;

@end

NS_ASSUME_NONNULL_END
