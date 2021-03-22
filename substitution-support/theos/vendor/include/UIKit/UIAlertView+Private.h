@interface UIAlertView (Private)

@property (nonatomic, retain) NSString *context;
@property (nonatomic, retain, readonly) UITextField *textField;

@property BOOL forceHorizontalButtonsLayout;
@property NSInteger numberOfRows;

@end
