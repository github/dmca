#import "HBLOHandlerDelegate.h"

NS_ASSUME_NONNULL_BEGIN

/**
 * A handler is a class that is consulted by Opener when the system is about to
 * open a URL. If a handler returns an NSURL from openURL:sender:, this URL will
 * be used instead. If the handler doesn’t want to replace the URL, it returns
 * nil.
 *
 * Opener handles enabling and disabling of the handler for you. If you
 * would like to override this and provide your own preferences, you can set
 * preferencesBundle and preferencesClass.
 */
@interface HBLOHandler : NSObject <HBLOHandlerDelegate>

/**
 * Name of the handler, shown in Settings.
 */
@property (nonatomic, retain) NSString *name;

/**
 * Identifier of the handler, used internally by Opener.
 *
 * To prevent conflicts, use a reverse DNS name, such as your package’s
 * identifier. If your handler previously supported Opener versions prior to
 * 2.0, use the name you passed to -[HBLibOpener registerHandlerWithName:block:].
 */
@property (nonatomic, retain) NSString *identifier;

/**
 * Custom Preferences list controller bundle.
 *
 * If nil, a switch will be shown in Settings, and enabled/disabled state is
 * handled by Opener. Otherwise, tapping the cell will push your list
 * controller, and enabled state is handled by your code.
 *
 * @see preferencesClass
 */
@property (nonatomic, retain, nullable) NSBundle *preferencesBundle;

/**
 * Custom Preferences list controller class.
 *
 * Refer to preferencesBundle for more details. If this is nil, the principal
 * class of the bundle will be used.
 *
 * @see preferencesBundle
 */
@property (nonatomic, retain, nullable) NSString *preferencesClass;

/**
 * Called when a URL is opened in order to check if your handler has an override
 * of the URL.
 *
 * @param url The original URL that the user wants to open.
 * @param sender The bundle identifier of the app that invoked the URL open
 * request. If the request was invoked through SpringBoard or a non-graphical
 * program such as `sbopenurl`, this argument is nil.
 * @returns An NSURL that will be opened instead of the original, an NSArray of
 * NSURLs for multiple apps, or nil to not change the URL.
 */
- (nullable id)openURL:(NSURL *)url sender:(nullable NSString *)sender;

@end

NS_ASSUME_NONNULL_END
