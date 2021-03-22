//
//  Created by Marco Arment on 2014-04-06.
//  Copyright (c) 2014 Marco Arment. See included LICENSE file.
//

#import "NSLayoutConstraint+CompactConstraint.h"

/**
 * UIView (CompactConstraint), a class category from Marco Arment’s
 * [CompactConstraint](https://github.com/marcoarment/CompactConstraint)
 * library, is integrated with Cephei. CompactConstraint provides an Auto Layout
 * grammar and methods that are easier to use and understand than UIKit’s built
 * in functions for programmatically adding constraints.
 *
 * Refer to
 * [its readme](https://github.com/marcoarment/CompactConstraint/blob/master/README.md)
 * to learn how to use it. There are two changes to note: most importantly, the
 * methods have an `hb_` prefix, and two methods that are marked as deprecated
 * in the original project have been removed.
 *
 * CompactConstraint is licensed under the MIT License.
 */
@interface UIView (CompactConstraint)

// Add a single constraint with the compact syntax
- (NSLayoutConstraint *)hb_addCompactConstraint:(NSString *)relationship metrics:(NSDictionary <NSString *, NSNumber *> *)metrics views:(NSDictionary <NSString *, UIView *> *)views;

// Add any number of constraints. Can also mix in Visual Format Language strings.
- (NSArray *)hb_addCompactConstraints:(NSArray *)relationshipStrings metrics:(NSDictionary <NSString *, NSNumber *> *)metrics views:(NSDictionary <NSString *, UIView *> *)views;

// And a convenient shortcut for what we always end up doing with the visualFormat call.
- (void)hb_addConstraintsWithVisualFormat:(NSString *)format options:(NSLayoutFormatOptions)opts metrics:(NSDictionary <NSString *, NSNumber *> *)metrics views:(NSDictionary <NSString *, UIView *> *)views;

@end
