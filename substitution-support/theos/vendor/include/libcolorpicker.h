#import  <UIKit/UIKit.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
UIColor *LCPParseColorString(NSString *colorStringFromPrefs, NSString *colorStringFallback);
//old DONT USE
UIColor *colorFromDefaultsWithKey(NSString *defaults, NSString *key, NSString *fallback);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

@interface UIColor (PFColor)
+ (UIColor *)PF_colorWithHex:(NSString *)hexString;
+ (NSString *)hexFromColor:(UIColor *)color;
@property (nonatomic, assign, readonly) CGFloat alpha;
@property (nonatomic, assign, readonly) CGFloat red;
@property (nonatomic, assign, readonly) CGFloat green;
@property (nonatomic, assign, readonly) CGFloat blue;
@property (nonatomic, assign, readonly) CGFloat hue;
@property (nonatomic, assign, readonly) CGFloat saturation;
@property (nonatomic, assign, readonly) CGFloat brightness;
- (UIColor *)desaturate:(CGFloat)percent;
- (UIColor *)lighten:(CGFloat)percent;
- (UIColor *)darken:(CGFloat)percent;
@end

@interface PFColorAlert : NSObject
// DO NOT USE OLD METHOD
//- (void)showWithStartColor:(UIColor *)startColor showAlpha:(BOOL)showAlpha completion:(void (^)(UIColor *pickedColor))completionBlock;
+ (PFColorAlert *)colorAlertWithStartColor:(UIColor *)startColor showAlpha:(BOOL)showAlpha;
- (PFColorAlert *)initWithStartColor:(UIColor *)startColor showAlpha:(BOOL)showAlpha;
- (void)displayWithCompletion:(void (^)(UIColor *pickedColor))fcompletionBlock;
- (void)close;
@end

@interface PFLiteColorCell : UITableViewCell
- (id)initWithStyle:(long long)style reuseIdentifier:(id)identifier specifier:(id)specifier;
- (UIColor *)previewColor; // this will be used for the circle preview view. override in a subclass
- (id)specifier;
- (void)updateCellDisplay;
@end

@interface PFSimpleLiteColorCell : PFLiteColorCell
@end
