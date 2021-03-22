#define kBKSBackgroundModeUnboundedTaskCompletion @"unboundedTaskCompletion"
#define kBKSBackgroundModeContinuous              @"continuous"
#define kBKSBackgroundModeFetch                   @"fetch"
#define kBKSBackgroundModeRemoteNotification      @"remote-notification"
#define kBKSBackgroundModeExternalAccessory       @"external-accessory"
#define kBKSBackgroundModeVoIP                    @"voip"
#define kBKSBackgroundModeLocation                @"location"
#define kBKSBackgroundModeAudio                   @"audio"
#define kBKSBackgroundModeBluetoothCentral        @"bluetooth-central"
#define kBKSBackgroundModeBluetoothPeripheral     @"bluetooth-peripheral"

typedef NS_ENUM(NSUInteger, BKSProcessAssertionReason) {
	BKSProcessAssertionReasonNone = 0,
	BKSProcessAssertionReasonAudio = 1,
	BKSProcessAssertionReasonLocation = 2,
	BKSProcessAssertionReasonExternalAccessory = 3,
	BKSProcessAssertionReasonFinishTask = 4,
	BKSProcessAssertionReasonBluetooth = 5,
	BKSProcessAssertionReasonNetworkAuthentication = 6,
	BKSProcessAssertionReasonBackgroundUI = 7,
	BKSProcessAssertionReasonInterAppAudioStreaming = 8,
	BKSProcessAssertionReasonViewServices = 9,
	BKSProcessAssertionReasonNewsstandDownload = 10,
	BKSProcessAssertionReasonBackgroundDownload = 11,
	BKSProcessAssertionReasonVOiP = 12,
	BKSProcessAssertionReasonExtension = 13,
	BKSProcessAssertionReasonContinuityStreams = 14,
	BKSProcessAssertionReasonHealthKit = 15,
	BKSProcessAssertionReasonWatchConnectivity = 16,
	BKSProcessAssertionReasonSnapshot = 17,
	BKSProcessAssertionReasonComplicationUpdate = 18,
	BKSProcessAssertionReasonWorkoutProcessing = 19,
	BKSProcessAssertionReasonComplicationPushUpdate = 20,
	// 21-9999 unknown
	BKSProcessAssertionReasonActivation = 10000,
	BKSProcessAssertionReasonSuspend = 10001,
	BKSProcessAssertionReasonTransientWakeup = 10002,
	BKSProcessAssertionReasonVOiP_PreiOS8 = 10003,
	BKSProcessAssertionReasonPeriodicTask_iOS8 = BKSProcessAssertionReasonVOiP_PreiOS8,
	BKSProcessAssertionReasonFinishTaskUnbounded = 10004,
	BKSProcessAssertionReasonContinuous = 10005,
	BKSProcessAssertionReasonBackgroundContentFetching = 10006,
	BKSProcessAssertionReasonNotificationAction = 10007,
	BKSProcessAssertionReasonPIP = 10008,
	// 10009-49999 unknown
	BKSProcessAssertionReasonFinishTaskAfterBackgroundContentFetching = 50000,
	BKSProcessAssertionReasonFinishTaskAfterBackgroundDownload = 50001,
	BKSProcessAssertionReasonFinishTaskAfterPeriodicTask = 50002,
	BKSProcessAssertionReasonAfterNoficationAction = 50003,
	BKSProcessAssertionReasonFinishTaskAfterWatchConnectivity = 50004,
	// 50005+ unknown
};

typedef NS_ENUM(NSUInteger, ProcessAssertionFlags) {
	BKSProcessAssertionFlagNone = 0,
	BKSProcessAssertionFlagPreventSuspend         = 1 << 0,
	BKSProcessAssertionFlagPreventThrottleDownCPU = 1 << 1,
	BKSProcessAssertionFlagAllowIdleSleep         = 1 << 2,
	BKSProcessAssertionFlagWantsForegroundResourcePriority  = 1 << 3
};

@interface BKSProcessAssertion : NSObject

- (instancetype)initWithPID:(NSInteger)pid flags:(NSUInteger)flags reason:(NSUInteger)reason name:(NSString *)name withHandler:(id)handler;

- (instancetype)initWithBundleIdentifier:(NSString *)identifier flags:(NSUInteger)flags reason:(NSUInteger)reason name:(NSString *)name withHandler:(id)handler;

+ (NSString *)NameForReason:(NSUInteger)reason;

- (BOOL)valid;
- (void)invalidate;

@end
