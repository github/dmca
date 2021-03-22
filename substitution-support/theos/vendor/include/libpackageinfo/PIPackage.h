/**
 * Name: libpackageinfo
 * Type: iOS library
 * Desc: iOS library for retrieving information regarding installed packages.
 *
 * Author: Lance Fetters (aka. ashikase)
 * License: LGPL v3 (See LICENSE file for details)
 */

#import <Foundation/Foundation.h>

@interface PIPackage : NSObject {
    @package NSMutableDictionary *packageDetails_;
}
@property(nonatomic, readonly) NSString *identifier;
@property(nonatomic, readonly) NSString *storeIdentifier;
@property(nonatomic, readonly) NSString *name;
@property(nonatomic, readonly) NSString *author;
@property(nonatomic, readonly) NSString *maintainer;
@property(nonatomic, readonly) NSString *version;
@property(nonatomic, readonly) NSDate *installDate;
@property(nonatomic, readonly) NSString *bundlePath;
@property(nonatomic, readonly) NSString *libraryPath;
+ (instancetype)packageForFile:(NSString *)filepath;
+ (instancetype)packageWithIdentifier:(NSString *)identifier;
- (id)initWithDetails:(NSDictionary *)details;
- (id)initWithDetailsFromJSONString:(NSString *)string;
- (NSDictionary *)dictionaryRepresentation;
- (NSString *)JSONRepresentation;
@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
