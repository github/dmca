#import "APSMessage.h"

@interface APSIncomingMessage : APSMessage

@property (getter=wasFromStorage, nonatomic) BOOL fromStorage;
@property (getter=wasLastMessageFromStorage, nonatomic) BOOL lastMessageFromStorage;
@property (nonatomic) NSInteger priority;
@property (nonatomic, copy) NSDate *timestamp;
@property (nonatomic, copy) NSData *token;

@end
