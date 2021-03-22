extern NSString *const _AVSystemController_HeadphoneJackIsConnectedDidChangeNotification;
extern NSString *const _AVSystemController_HeadphoneJackIsConnectedAttribute;

@interface AVSystemController : NSObject

+ (instancetype)sharedAVSystemController;

- (id)attributeForKey:(id)key;

@end
