//
//  Created by Marco Arment on 2014-04-06.
//  Copyright (c) 2014 Marco Arment. See included LICENSE file.
//

/**
 * NSLayoutConstraint (CompactConstraint), a class category from Marco Arment’s
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
@interface NSLayoutConstraint (CompactConstraint)

+ (instancetype)hb_compactConstraint:(NSString *)relationship metrics:(NSDictionary <NSString *, NSNumber *> *)metrics views:(NSDictionary <NSString *, UIView *> *)views self:(id)selfView;
+ (NSArray <NSLayoutConstraint *> *)hb_compactConstraints:(NSArray <NSString *> *)relationshipStrings metrics:(NSDictionary <NSString *, NSNumber *> *)metrics views:(NSDictionary <NSString *, UIView *> *)views self:(id)selfView;

// And a convenient shortcut for creating constraints with the visualFormat string as the identifier
+ (NSArray *)hb_identifiedConstraintsWithVisualFormat:(NSString *)format options:(NSLayoutFormatOptions)opts metrics:(NSDictionary <NSString *, NSNumber *> *)metrics views:(NSDictionary <NSString *, UIView *> *)views;

@end
