@interface MPUNowPlayingMetadata : NSObject

@property (nonatomic, readonly) NSString *title;
@property (nonatomic, readonly) NSString *album;
@property (nonatomic, readonly) NSString *artist;

@property (nonatomic, readonly) double duration;
@property (nonatomic, readonly) double elapsedTime;

@end
