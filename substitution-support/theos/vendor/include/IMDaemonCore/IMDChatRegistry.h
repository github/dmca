@class IMDChat;

@interface IMDChatRegistry : NSObject

+ (instancetype)sharedInstance;

- (NSArray <IMDChat *> *)chats;

@end
