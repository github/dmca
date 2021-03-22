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

@interface TSEmailInstruction : TSInstruction
@property(nonatomic, readonly) NSString *toAddress;
@property(nonatomic, readonly) NSArray *ccAddresses;
@property(nonatomic, readonly) NSString *subject;
@property(nonatomic, readonly) BOOL isSupport;
@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
