#include <sys/cdefs.h>

@class BBBulletinRequest;
@protocol BBDataProvider;

@interface BBServer : NSObject

- (void)_addDataProvider:(BBDataProvider *)dataProvider sortSectionsNow:(BOOL)sortSections;

@end

__BEGIN_DECLS

extern void BBDataProviderAddBulletin(BBDataProvider *dataProvider, BBBulletinRequest *bulletinRequest);
extern void BBDataProviderWithdrawBulletinsWithRecordID(BBDataProvider *dataProvider, NSString *recordID);

__END_DECLS
