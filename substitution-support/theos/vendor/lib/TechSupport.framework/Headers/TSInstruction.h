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

@class TSPackage;

@interface TSInstruction : NSObject
@property(nonatomic, copy) NSString *title;
@property(nonatomic, readonly) NSArray *tokens;
+ (instancetype)instructionWithString:(NSString *)string;
+ (NSArray *)instructionsWithString:(NSString *)string;
+ (void)flushInstructions;
- (instancetype)initWithTokens:(NSArray *)tokens;
- (NSComparisonResult)compare:(TSInstruction *)other;
@end

NSString *stripQuotes(NSString *string);

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
