/**
 * Name: libpackageinfo
 * Type: iOS library
 * Desc: iOS library for retrieving information regarding installed packages.
 *
 * Author: Lance Fetters (aka. ashikase)
 * License: LGPL v3 (See LICENSE file for details)
 */

#import <Foundation/Foundation.h>

@class PIPackage;

@interface PIPackageCache : NSObject
+ (instancetype)sharedCache;
- (PIPackage *)packageForFile:(NSString *)filepath;
- (PIPackage *)packageWithIdentifier:(NSString *)identifier;
- (void)cachePackage:(PIPackage *)package forFile:(NSString *)filepath;
- (void)cachePackage:(PIPackage *)package forIdentifier:(NSString *)identifier;
- (void)removeAllObjects;
@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
