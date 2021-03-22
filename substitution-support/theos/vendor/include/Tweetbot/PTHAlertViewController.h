@class PTHViewController;

@interface PTHAlertViewController : PTHViewController

- (void)runModal:(id)idontknowwhatthisis;

@property (nonatomic, retain) NSString *title;
@property (nonatomic, retain) NSString *message;
@property (nonatomic, retain, setter=setOKButtonTitle:) NSString *okButtonTitle;
@property (nonatomic, retain) id block;

@end
