/**
 * Name: TechSupport
 * Type: iOS framework
 * Desc: iOS framework to assist in providing support for and receiving issue
 *       reports and feedback from users.
 *
 * Author: Lance Fetters (aka. ashikase)
 * License: LGPL v3 (See LICENSE file for details)
 */

#import <Foundation/Foundation.h>

@class TSIncludeInstruction;
@class TSLinkInstruction;

@interface TSPackage : NSObject
@property(nonatomic, readonly) NSString *identifier;
@property(nonatomic, readonly) NSString *storeIdentifier;
@property(nonatomic, readonly) NSString *name;
@property(nonatomic, readonly) NSString *author;
@property(nonatomic, readonly) NSString *version;
@property(nonatomic, readonly) NSDate *installDate;
@property(nonatomic, readonly) BOOL isAppStore;
@property(nonatomic, readonly) TSLinkInstruction *storeLink;
@property(nonatomic, readonly) TSLinkInstruction *supportLink;
@property(nonatomic, readonly) NSArray *otherLinks;
@property(nonatomic, readonly) TSIncludeInstruction *preferencesAttachment;
@property(nonatomic, readonly) NSArray *otherAttachments;
+ (instancetype)packageForFile:(NSString *)path;
+ (instancetype)packageWithIdentifier:(NSString *)identifier;
- (instancetype)initForFile:(NSString *)path;
- (instancetype)initWithIdentifier:(NSString *)identifier;
@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
