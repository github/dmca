#import "LAActivatorVersion.h"

// Events represent an assignable gesture that has or could occurred

@interface LAEvent : NSObject<NSCoding> LA_PRIVATE_IVARS(LAEvent)

+ (id)eventWithName:(NSString *)name;
+ (id)eventWithName:(NSString *)name mode:(NSString *)mode;
- (id)initWithName:(NSString *)name;
- (id)initWithName:(NSString *)name mode:(NSString *)mode;

@property (nonatomic, readonly) NSString *name;
@property (nonatomic, readonly) NSString *mode;
@property (nonatomic, getter=isHandled) BOOL handled;
@property (nonatomic, copy) NSDictionary *userInfo;

@end

extern NSString * const LAEventNameMenuPressSingle;
extern NSString * const LAEventNameMenuPressDouble;
extern NSString * const LAEventNameMenuPressTriple;
extern NSString * const LAEventNameMenuHoldShort;
extern NSString * const LAEventNameMenuHoldLong;

extern NSString * const LAEventNameLockHoldShort;
extern NSString * const LAEventNameLockHoldLong;
extern NSString * const LAEventNameLockPressDouble;
extern NSString * const LAEventNameLockPressWithMenu;

extern NSString * const LAEventNameSpringBoardPinch;
extern NSString * const LAEventNameSpringBoardSpread;

extern NSString * const LAEventNameStatusBarSwipeRight;
extern NSString * const LAEventNameStatusBarSwipeLeft;
extern NSString * const LAEventNameStatusBarTapDouble;
extern NSString * const LAEventNameStatusBarTapDoubleLeft;
extern NSString * const LAEventNameStatusBarTapDoubleRight;
extern NSString * const LAEventNameStatusBarTapSingle;
extern NSString * const LAEventNameStatusBarTapSingleLeft;
extern NSString * const LAEventNameStatusBarTapSingleRight;
extern NSString * const LAEventNameStatusBarHold;
extern NSString * const LAEventNameStatusBarHoldLeft;
extern NSString * const LAEventNameStatusBarHoldRight;

extern NSString * const LAEventNameVolumeDownUp;
extern NSString * const LAEventNameVolumeUpDown;
extern NSString * const LAEventNameVolumeDisplayTap;
extern NSString * const LAEventNameVolumeToggleMuteTwice;
extern NSString * const LAEventNameVolumeDownHoldShort;
extern NSString * const LAEventNameVolumeUpHoldShort;
extern NSString * const LAEventNameVolumeDownPress;
extern NSString * const LAEventNameVolumeUpPress;
extern NSString * const LAEventNameVolumeBothPress;

extern NSString * const LAEventNameSlideInFromBottom;
extern NSString * const LAEventNameSlideInFromBottomLeft;
extern NSString * const LAEventNameSlideInFromBottomRight;
extern NSString * const LAEventNameSlideInFromLeft;
extern NSString * const LAEventNameSlideInFromRight;
extern NSString * const LAEventNameStatusBarSwipeDown; // Now a slide gesture in iOS5.0+; extern and name kept for backwards compatibility
#define LAEventNameSlideInFromTop LAEventNameStatusBarSwipeDown
extern NSString * const LAEventNameSlideInFromTopLeft;
extern NSString * const LAEventNameSlideInFromTopRight;

extern NSString * const LAEventNameTwoFingerSlideInFromBottom;
extern NSString * const LAEventNameTwoFingerSlideInFromBottomLeft;
extern NSString * const LAEventNameTwoFingerSlideInFromBottomRight;
extern NSString * const LAEventNameTwoFingerSlideInFromLeft;
extern NSString * const LAEventNameTwoFingerSlideInFromRight;
extern NSString * const LAEventNameTwoFingerSlideInFromTop;
extern NSString * const LAEventNameTwoFingerSlideInFromTopLeft;
extern NSString * const LAEventNameTwoFingerSlideInFromTopRight;

extern NSString * const LAEventNameDragOffBottom;
extern NSString * const LAEventNameDragOffLeft;
extern NSString * const LAEventNameDragOffRight;
extern NSString * const LAEventNameDragOffTop;

extern NSString * const LAEventScreenBottomSwipeLeft;
extern NSString * const LAEventScreenBottomSwipeRight;
extern NSString * const LAEventScreenLeftSwipeDown;
extern NSString * const LAEventScreenLeftSwipeUp;
extern NSString * const LAEventScreenRightSwipeDown;
extern NSString * const LAEventScreenRightSwipeUp;

extern NSString * const LAEventNameMotionShake;

extern NSString * const LAEventNameHeadsetButtonPressSingle;
extern NSString * const LAEventNameHeadsetButtonHoldShort;
extern NSString * const LAEventNameHeadsetConnected;
extern NSString * const LAEventNameHeadsetDisconnected;

extern NSString * const LAEventNameLockScreenClockDoubleTap;
extern NSString * const LAEventNameLockScreenClockTapHold;
extern NSString * const LAEventNameLockScreenClockSwipeLeft;
extern NSString * const LAEventNameLockScreenClockSwipeRight;
extern NSString * const LAEventNameLockScreenClockSwipeDown;

extern NSString * const LAEventNamePowerConnected;
extern NSString * const LAEventNamePowerDisconnected;

extern NSString * const LAEventNameThreeFingerTap;
extern NSString * const LAEventNameThreeFingerPinch;
extern NSString * const LAEventNameThreeFingerSpread;

extern NSString * const LAEventNameFourFingerTap;
extern NSString * const LAEventNameFourFingerPinch;
extern NSString * const LAEventNameFourFingerSpread;

extern NSString * const LAEventNameFiveFingerTap;
extern NSString * const LAEventNameFiveFingerPinch;
extern NSString * const LAEventNameFiveFingerSpread;

extern NSString * const LAEventNameClamshellOpen;
extern NSString * const LAEventNameClamshellClose;

extern NSString * const LAEventNameSpringBoardIconFlickUp;
extern NSString * const LAEventNameSpringBoardIconFlickDown;
extern NSString * const LAEventNameSpringBoardIconFlickLeft;
extern NSString * const LAEventNameSpringBoardIconFlickRight;

extern NSString * const LAEventNameDeviceLocked;
extern NSString * const LAEventNameDeviceUnlocked;

extern NSString * const LAEventNameNetworkJoinedWiFi;
extern NSString * const LAEventNameNetworkLeftWiFi;

extern NSString * const LAEventNameFingerprintSensorPressSingle;

// Icon Gestures
extern NSString * const LAEventUserInfoDisplayIdentifier;
extern NSString * const LAEventUserInfoIconView;

extern NSString * const LAEventUserInfoUnlockedDeviceToSendEvent;
