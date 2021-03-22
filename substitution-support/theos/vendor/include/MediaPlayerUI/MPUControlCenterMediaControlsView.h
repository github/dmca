#import "MPUMediaRemoteControlsView.h"

@class MPUTransportControlsView;

@interface MPUControlCenterMediaControlsView : MPUMediaRemoteControlsView

@property (nonatomic, strong, readonly) MPUTransportControlsView *transportControls;

@end
