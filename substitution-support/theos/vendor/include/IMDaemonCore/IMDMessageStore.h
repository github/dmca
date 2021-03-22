@class FZMessage;

@interface IMDMessageStore : NSObject

+ (instancetype)sharedInstance;

- (FZMessage *)messageWithGUID:(NSString *)guid;

@end
