#import <Foundation/Foundation.h>

typedef enum {
	FSSwitchStateOff = 0,
	FSSwitchStateOn = 1,
	FSSwitchStateIndeterminate = -1
} FSSwitchState;

extern NSString *NSStringFromFSSwitchState(FSSwitchState state);
extern FSSwitchState FSSwitchStateFromNSString(NSString *stateString);
