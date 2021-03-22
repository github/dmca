@interface IMItem : NSObject

@property (nonatomic) long long type;
@property (nonatomic) long long messageID;
@property (nonatomic, retain) NSString *unformattedID;
@property (nonatomic, retain) NSString *service;

@property (nonatomic, retain) NSString *account;
@property (nonatomic, retain) NSString *accountID;
@property (nonatomic, retain) NSString *handle;
@property (nonatomic, retain) NSString *countryCode;
@property (nonatomic, retain) NSString *guid;

@property (nonatomic, retain) NSString *roomName;
@property (nonatomic, retain) NSString *sender;
@property (nonatomic, retain) NSDictionary *senderInfo;

@property (nonatomic, readonly) BOOL isFromMe;
@property (nonatomic, retain) id context;
@property (nonatomic, retain) NSDate *time;

@end
