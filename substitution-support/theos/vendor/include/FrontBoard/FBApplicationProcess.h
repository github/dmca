#import "FBProcess.h"

@class BKSProcess;

@interface FBApplicationProcess : FBProcess {
    BKSProcess *_bksProcess;
}

+ (void)deleteAllJobs;

- (void)killForReason:(NSInteger)reason andReport:(BOOL)report withDescription:(NSString *)description;

@property (nonatomic, assign, getter=isRecordingAudio) BOOL recordingAudio;
@property (nonatomic, assign, getter=isNowPlayingWithAudio) BOOL nowPlayingWithAudio;

- (void)processWillExpire:(BKSProcess *)process;

@end
