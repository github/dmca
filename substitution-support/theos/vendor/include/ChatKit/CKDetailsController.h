#import "CKScrollViewController.h"

@class CKDetailsTableView, CKConversation;

@interface CKDetailsController : CKScrollViewController

@property (nonatomic, retain) CKDetailsTableView *tableView;

@property (nonatomic, retain) CKConversation *conversation;

@end
