#import "IMItem.h"

typedef NS_OPTIONS(unsigned long long, IMMessageItemFlags) {
	IMMessageItemFlagsFinished = 0x1
};

@interface IMMessageItem : IMItem

@property (nonatomic, retain) NSDate *timeDelivered;
@property (nonatomic, retain) NSDate *timePlayed;
@property (nonatomic, retain) NSDate *timeRead;

@property (nonatomic, retain) NSString *subject;
@property (nonatomic, retain) NSAttributedString *body;
@property (nonatomic, retain) NSData *bodyData;
@property (nonatomic, retain) NSString *plainBody;

@property (nonatomic) IMMessageItemFlags flags;
@property (nonatomic) NSUInteger errorCode;
@property (nonatomic) long long expireState;
@property (nonatomic) long long replaceID;
@property (nonatomic) BOOL hasDataDetectorResults;
@property (nonatomic, retain) NSArray *fileTransferGUIDs;

@property (nonatomic, readonly) BOOL isAlert;
@property (nonatomic, readonly) BOOL isAudioMessage;
@property (nonatomic, readonly) BOOL isDelivered;
@property (nonatomic, readonly) BOOL isEmote;
@property (nonatomic, readonly) BOOL isEmpty;
@property (nonatomic, readonly) BOOL isExpirable;
@property (nonatomic, readonly) BOOL isFinished;
@property (nonatomic, readonly) BOOL isFromExternalSource;
@property (nonatomic, readonly) BOOL isLocatingMessage;
@property (nonatomic, readonly) BOOL isPlayed;
@property (nonatomic, readonly) BOOL isPrepared;
@property (nonatomic, readonly) BOOL isRead;
@property (nonatomic, readonly) BOOL isSent;
@property (nonatomic, readonly) BOOL isTypingMessage;
@property (nonatomic, readonly) BOOL wasDataDetected;
@property (nonatomic, readonly) BOOL wasDowngraded;

@end
