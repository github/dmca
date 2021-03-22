@interface SBDefaultBannerTextView : UIView

- (BOOL)textWillWrapForWidth:(CGFloat)width;

@property (nonatomic, retain) NSString *secondaryText;

@end
