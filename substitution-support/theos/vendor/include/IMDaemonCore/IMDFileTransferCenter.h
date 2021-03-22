@class IMFileTransfer;

@interface IMDFileTransferCenter : NSObject

- (IMFileTransfer *)transferForGUID:(NSString *)guid;

@end
