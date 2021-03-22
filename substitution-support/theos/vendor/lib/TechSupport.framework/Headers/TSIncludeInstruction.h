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

typedef enum {
    TSIncludeInstructionTypeFile,
    TSIncludeInstructionTypePlist,
    TSIncludeInstructionTypeCommand
} TSIncludeInstructionType;

@interface TSIncludeInstruction : TSInstruction
@property(nonatomic, readonly) NSData *content;
@property(nonatomic, readonly) NSString *command;
@property(nonatomic, readonly) NSString *filepath;
@property(nonatomic, readonly) NSString *mimeType;
@property(nonatomic, readonly) TSIncludeInstructionType includeType;
@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
