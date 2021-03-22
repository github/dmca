/**
 * Name: TechSupport
 * Type: iOS framework
 * Desc: iOS framework to assist in providing support for and receiving issue
 *       reports and feedback from users.
 *
 * Author: Lance Fetters (aka. ashikase)
 * License: LGPL v3 (See LICENSE file for details)
 */

#import <UIKit/UIKit.h>

@class TSLinkInstruction;
@class TSPackage;

@interface TSContactViewController : UIViewController

/**
 * The string that is displayed when there is not input from the user.
 *
 * This value is _nil_ by default. When nil, a default placeholder string will
 * be displayed. The default string is localized.
 */
@property(nonatomic, copy) NSString *detailEntryPlaceholderText;

/**
 * The string used for the subject of the generated email.
 *
 * This value is _nil_ by default.
 */
@property(nonatomic, copy) NSString *subject;

/**
 * The string displayed in the generated email, below the device details and
 * above the user-entered details.
 *
 * This value is _nil_ by default.
 */
@property(nonatomic, copy) NSString *messageBody;

/**
 * A string used for formatting the details input by the user.
 *
 * This value is _nil_ by default. Any set value *must* include a "%@" to
 * represent where the user input will be inserted.
 */
@property(nonatomic, copy) NSString *detailFormat;

/**
 * A string representing the generator of this email.
 *
 * This value is _nil_ by default.
 */
@property(nonatomic, copy) NSString *byline;

/**
 * A Boolean value that determines whether a user must enter details.
 *
 * This value is NO by default. When YES, the user will not be able to generate
 * the email without entering text into the detail area.
 */
@property(nonatomic, assign) BOOL requiresDetailsFromUser;

/**
 * Initializer.
 *
 * @param package The package to generate an email for.
 * @param linkInstruction The instruction containing the email command.
 * @param includeInstructions Array of TSIncludeInstruction objects specifying
 *        what to attach to the generated email.
 *
 * @return A new TSContactViewController instance.
 */
- (id)initWithPackage:(TSPackage *)package linkInstruction:(TSLinkInstruction *)linkInstruction includeInstructions:(NSArray *)includeInstructions;

/**
 * Initializer.
 *
 * @param package The package to generate an email for.
 * @param instructions Array of TSInstruction objects specifying
 *        to whom to send and what to attach to the generated email.
 *
 * @return A new TSContactViewController instance.
 */
- (id)initWithPackage:(TSPackage *)package instructions:(NSArray *)instructions;

@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
