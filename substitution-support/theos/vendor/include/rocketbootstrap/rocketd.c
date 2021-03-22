#import <notify.h>
#import <CoreFoundation/CFLogUtilities.h>
#define LIGHTMESSAGING_USE_ROCKETBOOTSTRAP 0

#import "rocketbootstrap_internal.h"

static CFMutableSetRef allowedNames;

static const uint32_t one = 1;

static void machPortCallback(CFMachPortRef port, void *bytes, CFIndex size, void *info)
{
	LMMessage *request = bytes;
	if (!LMDataWithSizeIsValidMessage(bytes, size)) {
		LMSendReply(request->head.msgh_remote_port, NULL, 0);
		LMResponseBufferFree(bytes);
		return;
	}
	// Send Response
	const void *data = LMMessageGetData(request);
	size_t length = LMMessageGetDataLength(request);
	const void *reply_data = NULL;
	uint32_t reply_length = 0;
	if (length) {
		CFStringRef name = CFStringCreateWithBytes(kCFAllocatorDefault, data, length, kCFStringEncodingUTF8, false);
		if (name) {
			switch (request->head.msgh_id) {
				case 0: // Register
#ifdef DEBUG
					CFLog(kCFLogLevelWarning, CFSTR("Unlocking %@"), name);
#endif
					CFSetAddValue(allowedNames, name);
					break;
				case 1: // Query
					if (CFSetContainsValue(allowedNames, name)) {
						reply_data = &one;
						reply_length = sizeof one;
#ifdef DEBUG
						CFLog(kCFLogLevelWarning, CFSTR("Queried %@, is unlocked"), name);
#endif
					} else {
#ifdef DEBUG
						CFLog(kCFLogLevelWarning, CFSTR("Queried %@, is locked!"), name);
#endif
					}
					break;
				case 2:
					// Good morning, still awake
					reply_data = &one;
					reply_length = sizeof one;
					break;
			}
			CFRelease(name);
		}
	}
	LMSendReply(request->head.msgh_remote_port, reply_data, reply_length);
	LMResponseBufferFree(bytes);
}

int main(int argc, char *argv[])
{
	allowedNames = CFSetCreateMutable(kCFAllocatorDefault, 0, &kCFTypeSetCallBacks);
	LMCheckInService(connection.serverName, CFRunLoopGetCurrent(), machPortCallback, NULL);
	notify_post("com.rpetrich.rocketd.started");
	CFRunLoopRun();
	return 0;
}
