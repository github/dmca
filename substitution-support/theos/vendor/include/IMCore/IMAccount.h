#include <sys/cdefs.h>

@class IMHandle, IMServiceImpl;

@interface IMAccount : NSObject

- (IMHandle *)imHandleWithID:(NSString *)id;

@property BOOL _isUsableForSending;

@end

__BEGIN_DECLS

extern "C" IMAccount *IMPreferredSendingAccountForAddressesWithFallbackService(NSArray *addresses, IMServiceImpl *service);

__END_DECLS
