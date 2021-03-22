/**
 * The `HBSupportController` class in `CepheiPrefs` provides a factory that
 * configures a `TSContactViewController` for immediate use. The
 * `TSContactViewController` class, from
 * [TechSupport](https://github.com/ashikase/TechSupport) by ashikase, provides
 * a text box for entering a message, as well as the ability to view or remove
 * pre-defined attachments. After tapping the submit button, the information is
 * provided to the next step, which most commonly is an email composer.
 *
 * By providing a link instruction, you can have the message sent to a different
 * email address or posted to a website. You can also provide one or more
 * support instructions, which allow more files to be attached. For more
 * information and for examples of `TSContactViewController` usage, refer to the
 * [demos directory](https://github.com/ashikase/TechSupport/tree/master/Demo)
 * in the TechSupport source.
 *
 * A `TSContactViewController` should be pushed on your view controller stack;
 * it should not be presented modally.
 */

@class TSContactViewController, TSLinkInstruction, TSIncludeInstruction;

NS_ASSUME_NONNULL_BEGIN

@interface HBSupportController : NSObject

/**
 * Initialises a `TSLinkInstruction` for the provided email address.
 *
 * @param emailAddress The email address to send an email to.
 * @returns A pre-configured instance of `TSLinkInstruction`.
 */
+ (TSLinkInstruction *)linkInstructionForEmailAddress:(NSString *)emailAddress;

/**
 * Initialises a `TSContactViewController` by using information provided by a
 * bundle.
 *
 * Refer to supportViewControllerForBundle:preferencesIdentifier:linkInstruction:supportInstructions:
 * for information on how the bundle is used.
 *
 * @param bundle A bundle included with the package.
 * @returns A pre-configured instance of `TSContactViewController`.
 * @see supportViewControllerForBundle:preferencesIdentifier:linkInstruction:supportInstructions:
 */
+ (TSContactViewController *)supportViewControllerForBundle:(NSBundle *)bundle;

/**
 * Initialises a `TSContactViewController` by using information provided by a
 * bundle and references identifier.
 *
 * Refer to supportViewControllerForBundle:preferencesIdentifier:linkInstruction:supportInstructions:
 * for information on how the bundle and preferences identifier are used.
 *
 * @param bundle A bundle included with the package.
 * @param preferencesIdentifier A preferences identifier that is used by the
 * package.
 * @returns A pre-configured instance of `TSContactViewController`.
 * @see supportViewControllerForBundle:preferencesIdentifier:linkInstruction:supportInstructions:
 */
+ (TSContactViewController *)supportViewControllerForBundle:(nullable NSBundle *)bundle preferencesIdentifier:(NSString *)preferencesIdentifier;

/**
 * Initialises a `TSContactViewController` by using information provided by
 * either a bundle or a preferences identifier, and providing it a custom link
 * instruction and support instructions.
 *
 * The bundle may set the key `HBPackageIdentifier` in its Info.plist,
 * containing the package identifier to gather information from. Otherwise, the
 * dpkg file lists are searched to find the package that contains the bundle.
 * The package’s name, identifier, and author will be used to fill out fields in
 * the information that the user will submit.
 *
 * Either a bundle or preferences identifier is required. If both are nil, an
 * exception will be thrown. If the `linkInstruction` argument is nil, a
 * `TSLinkInstruction` is derived from the `Author` field of the package’s
 * control file. `HBSupportController` implicitly adds the user’s package
 * listing (output of `dpkg -l`) and the preferences plist as attachments. To
 * add more, provide an array of `TSIncludeInstruction`s to the
 * `supportInstructions` argument.
 *
 * @param bundle A bundle included with the package.
 * @param preferencesIdentifier The preferences identifier of the package, if
 * it’s different from the package identifier that contains the bundle.
 * @param linkInstruction The link instruction to use, or nil.
 * @param supportInstructions Support instructions to use in combination with
 * the built-in ones defined by HBSupportController, or nil.
 * @returns A pre-configured instance of `TSContactViewController`.
 */
+ (TSContactViewController *)supportViewControllerForBundle:(nullable NSBundle *)bundle preferencesIdentifier:(nullable NSString *)preferencesIdentifier linkInstruction:(nullable TSLinkInstruction *)linkInstruction supportInstructions:(nullable NSArray <TSIncludeInstruction *> *)supportInstructions;

@end

NS_ASSUME_NONNULL_END
