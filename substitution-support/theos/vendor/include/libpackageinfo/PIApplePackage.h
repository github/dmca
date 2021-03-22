/**
 * Name: libpackageinfo
 * Type: iOS library
 * Desc: iOS library for retrieving information regarding installed packages.
 *
 * Author: Lance Fetters (aka. ashikase)
 * License: LGPL v3 (See LICENSE file for details)
 */

#import "PIPackage.h"

@interface PIApplePackage : PIPackage
@property(nonatomic, readonly) NSString *containerPath;
@end

/* vim: set ft=objc ff=unix sw=4 ts=4 tw=80 expandtab: */
