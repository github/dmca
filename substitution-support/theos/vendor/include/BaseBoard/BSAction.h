#import "BSDescriptionProviding.h"
#import "BSSettingDescriptionProvider.h"
#import "BSXPCCoding.h"

@interface BSAction : NSObject <BSDescriptionProviding, BSSettingDescriptionProvider, BSXPCCoding>

@end
