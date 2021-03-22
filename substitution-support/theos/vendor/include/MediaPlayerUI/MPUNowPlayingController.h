@class MPUNowPlayingController, MPUNowPlayingMetadata;

@protocol MPUNowPlayingDelegate <NSObject>

@optional

- (void)nowPlayingControllerDidBeginListeningForNotifications:(MPUNowPlayingController *)nowPlayingController;
- (void)nowPlayingControllerDidStopListeningForNotifications:(MPUNowPlayingController *)nowPlayingController;

- (void)nowPlayingController:(MPUNowPlayingController *)nowPlayingController nowPlayingApplicationDidChange:(NSString *)nowPlayingAppDisplayID;
- (void)nowPlayingController:(MPUNowPlayingController *)nowPlayingController nowPlayingInfoDidChange:(NSDictionary *)currentNowPlayingInfo;
- (void)nowPlayingController:(MPUNowPlayingController *)nowPlayingController playbackStateDidChange:(BOOL)isPlaying;
- (void)nowPlayingController:(MPUNowPlayingController *)nowPlayingController elapsedTimeDidChange:(double)elapsedTime;

@end

@interface MPUNowPlayingController : NSObject

@property (nonatomic) id <MPUNowPlayingDelegate> delegate;

- (void)startUpdating;
- (void)stopUpdating;

@property (nonatomic, readonly) BOOL isPlaying;
@property (nonatomic, readonly) MPUNowPlayingMetadata *currentNowPlayingMetadata;
@property (nonatomic, readonly) UIImage *currentNowPlayingArtwork;
@property (nonatomic, readonly) NSString *nowPlayingAppDisplayID;

@end
