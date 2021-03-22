/**
 * Name: TechSupport
 * Type: iOS framework
 * Desc: iOS framework to assist in providing support for and receiving issue
 *       reports and feedback from users.
 *
 * Author: Lance Fetters (aka. ashikase)
 * License: LGPL v3 (See LICENSE file for details)
 */

#import "TSInstruction.h"

@interface TSLinkInstruction : TSInstruction
@property(nonatomic, readonly) NSArray *recipients;
@property(nonatomic, readonly) NSString *unlocalizedTitle;
@property(nonatomic, readonly) NSURL *url;
@property(nonatomic, readonly) BOOL isEmail;
@property(nonatomic, readonly) BOOL isSupport;
@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
