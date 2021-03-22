/*

GSEvent.h ... Graphics Service Events.
 
Copyright (c) 2009, KennyTM~
All rights reserved.
 
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the KennyTM~ nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
*/

#ifndef GSEVENT_H
#define GSEVENT_H

#include "GSWindow.h"
#include <mach/message.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <Availability.h>

#if __cplusplus
extern "C" {
#endif

	typedef struct __GSEvent* GSEventRef;
	
	typedef struct GSPathInfo {
		unsigned char pathIndex;		// 0x0 = 0x5C
		unsigned char pathIdentity;		// 0x1 = 0x5D
		unsigned char pathProximity;	// 0x2 = 0x5E
		CGFloat pathPressure;				// 0x4 = 0x60
		CGFloat pathMajorRadius;		// 0x8 = 0x64
		CGPoint pathLocation;			// 0xC = 0x68
		GSWindowRef pathWindow;			// 0x14 = 0x70
	} GSPathInfo;	// sizeof = 0x18.
	
	typedef enum __GSHandInfoType {
		kGSHandInfoTypeTouchDown = 0,
		kGSHandInfoTypeTouchDragged = 1,
		kGSHandInfoTypeTouchMoved = 4,
		kGSHandInfoTypeTouchUp = 5,
		kGSHandInfoTypeCancel = 8
	} GSHandInfoType;
	
	// A.k.a. XXStruct_$jUSvD
	typedef struct GSHandInfo {
		GSHandInfoType type;	// 0x0 == 0x3C
		short deltaX, deltaY;	// 2, 4 = 0x40, 0x42
		float _0x44;
		float _0x48;
		float width;			// 0x10 == 0x4C
		float _0x50;
		float height;			// 0x18 == 0x54
		float _0x58;
		unsigned char _0x5C;
		unsigned char pathInfosCount;	// 0x22 == 0x5D
		GSPathInfo pathInfos[0];		// 0x60
	} GSHandInfo;	// sizeof = 0x24.
	
	typedef struct __GSScrollWheelInfo {
		int deltaY;
		int deltaX;
	} GSScrollWheelInfo;
	
	typedef struct __GSAccelerometerInfo {
		CGFloat axisX, axisY, axisZ;
	} GSAccelerometerInfo;
	
	typedef struct __GSDeviceOrientationInfo {
		int orientation;
	} GSDeviceOrientationInfo;
	
	typedef struct __GSKeyInfo {
		UniChar keyCode, characterIgnoringModifier, charCode;	// 0x3C, 0x3E, 0x40
		unsigned short characterSet;	// 0x42
		Boolean isKeyRepeating;	// 0x44
	} GSKeyInfo;
	
	typedef struct __GSHardwareKeyInfo {
		UniChar keyCode;	// 3c
		UniChar characterIgnoringModifier;	// 3e
		UniChar charCode;	// 40
		unsigned short characterSet;	// 42
		uint16_t characters_length;	// 44
		UniChar characters[32];		// 46 .. 84
		uint16_t unmodified_characters_length;	// 86
		UniChar unmodified_characters[32];		// 88 .. C6
		int unknown0 : 1;
		int isKeyVariant : 1;
		int unknown2 : 14;
		int unknown10 : 16;
	} GSHardwareKeyInfo;
	
	typedef struct __GSAccessoryKeyStateInfo {
		unsigned short a;
		int b;
	} GSAccessoryKeyStateInfo;
	
	typedef struct __GSAppPreferencesChangedInfo {
		size_t length;
		char appName[0];
	} GSAppPreferencesChangedInfo;
	
	typedef struct __GSResetIdleDurationInfo {
		int a, b;
	} GSResetIdleDurationInfo;
	
	typedef struct __GSEventProcessScriptInfo {
		int type;
		size_t length;
		char data[0];
	} GSEventProcessScriptInfo;
	
	typedef enum __GSEventType {
		kGSEventLeftMouseDown    = 1,
		kGSEventLeftMouseUp      = 2,
		kGSEventMouseMoved       = 5,
		kGSEventLeftMouseDragged = 6,
		
		kGSEventKeyDown = 10,
		kGSEventKeyUp = 11,
		kGSEventModifiersChanged = 12,
		kGSEventSimulatorKeyDown = 13,
		kGSEventHardwareKeyDown = 14,	// Maybe?
		kGSEventScrollWheel = 22,
		kGSEventAccelerate = 23,
		kGSEventProximityStateChanged = 24,
		kGSEventDeviceOrientationChanged = 50,
		kGSAppPreferencesChanged = 60,
		kGSEventUserDefaultsDidChange = 60,	// backward compatibility.
		
		kGSEventResetIdleTimer = 100,
		kGSEventResetIdleDuration = 101,
		kGSEventProcessScript = 200,
		kGSEventDumpUIHierarchy = 500,
		kGSEventDumpScreenContents = 501,
		
		kGSEventMenuButtonDown = 1000,
		kGSEventMenuButtonUp = 1001,
		kGSEventVolumeChanged = 1006,
		kGSEventVolumeUpButtonDown = 1006,
		kGSEventVolumeUpButtonUp = 1007,
		kGSEventVolumeDownButtonDown = 1008,
		kGSEventVolumeDownButtonUp = 1009,
		kGSEventLockButtonDown = 1010,
		kGSEventLockButtonUp = 1011,
		kGSEventRingerOff = 1012,
		kGSEventRingerOn = 1013,
		kGSEventRingerChanged = 1013,	// backward compatibility.
		kGSEventLockDevice = 1014,
		kGSEventStatusBarMouseDown = 1015,
		kGSEventStatusBarMouseDragged = 1016,
		kGSEventStatusBarMouseUp = 1017,
		kGSEventHeadsetButtonDown = 1018,
		kGSEventHeadsetButtonUp = 1019,
		kGSEventMotionBegin = 1020,
		kGSEventHeadsetAvailabilityChanged = 1021,
		kGSEventMediaKeyDown = 1022,	// ≥3.2
		kGSEventMediaKeyUp = 1023,	// ≥3.2
		
		kGSEventVibrate = 1100,
		kGSEventSetBacklightFactor = 1102,
		kGSEventSetBacklightLevel = 1103,
		
		kGSEventApplicationLaunch = 2000,
		kGSEventAnotherApplicationFinishedLaunching = 2001,
		kGSEventSetAppThreadPriority = 2002,
		kGSEventApplicationResume = 2003,		
		kGSEventApplicationDidEndResumeAnimation = 2004,
		kGSEventApplicationBeginSuspendAnimation = 2005,
		kGSEventApplicationHandleTestURL = 2006,
		kGSEventApplicationSuspendEventsOnly = 2007,
		kGSEventApplicationSuspend = 2008,
		kGSEventApplicationExit = 2009,
		kGSEventQuitTopApplication = 2010,
		kGSEventApplicationUpdateSuspendedSettings = 2011,
		
		kGSEventHand = 3001,
		
		kGSEventAccessoryAvailabilityChanged = 4000,
		kGSEventAccessoryKeyStateChanged = 4001,
		kGSEventAccessory = 4002,
		
		kGSEventOutOfLineDataRequest = 5000,
		kGSEventOutOfLineDataResponse = 5001,
		
		kGSEventUrgentMemoryWarning = 6000,
		
		kGSEventShouldRouteToFrontMost = 1<<17
	} GSEventType;
	
	typedef enum __GSEventSubType {
		kGSEventSubTypeUnknown,
	} GSEventSubType;
	
	typedef enum GSEventFlags {
		kGSEventFlagMaskShift     = 1 << 17,
		kGSEventFlagMaskControl   = 1 << 18,
		kGSEventFlagMaskAlternate = 1 << 19,
		kGSEventFlagMaskCommand   = 1 << 20
	} GSEventFlags;	
	
	typedef struct GSEventRecord {
		GSEventType type; // 0x8
		GSEventSubType subtype;	// 0xC
		CGPoint location; 	// 0x10
		CGPoint windowLocation;	// 0x18
		int windowContextId;	// 0x20
		uint64_t timestamp;	// 0x24, from mach_absolute_time
		GSWindowRef window;	// 0x2C
		GSEventFlags flags;	// 0x30
		unsigned senderPID;	// 0x34
		CFIndex infoSize; // 0x38
	} GSEventRecord;
	
#pragma mark -
#pragma mark General info
	
	CFTypeID GSEventGetTypeID();
	
	GSEventRef GSEventCopy(GSEventRef event);
	GSEventRef GSEventCreateWithEventRecord(const GSEventRecord* record);
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_0 && __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	GSEventRef GSEventCreateWithTypeAndLocation(GSEventType type, CGPoint location);
#endif
	GSEventRef GSEventCreateWithPlist(CFDictionaryRef dictionary) __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_2_2);
	
	const GSEventRecord* GSEventRecordGetRecordDataWithPlist(CFDictionaryRef plist) __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_2_2);
	void GSEventRecordGetRecordWithPlist(GSEventRef event_to_fill, CFDictionaryRef plist) __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_2_2);
	CFDictionaryRef GSEventCreatePlistRepresentation(GSEventRef event) __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_2_2);
	
	Boolean GSEventShouldRouteToFrontMost(GSEventRef event);
	void GSEventRemoveShouldRouteToFrontMost(GSEventRef event);

	GSEventType GSEventGetType(GSEventRef event);
	GSEventSubType GSEventGetSubType(GSEventRef event);
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_0 && __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	int GSEventGetWindowContextId(GSEventRef event);
#endif
	CGPoint GSEventGetLocationInWindow(GSEventRef event);
	CGPoint GSEventGetOuterMostPathPosition(GSEventRef event);
	CGPoint GSEventGetInnerMostPathPosition(GSEventRef event);
	CFAbsoluteTime GSEventGetTimestamp(GSEventRef event);
	GSWindowRef GSEventGetWindow(GSEventRef event);
	unsigned GSEventGetSenderPID(GSEventRef event) __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_3_0);

	const GSEventRecord* _GSEventGetGSEventRecord(GSEventRef event);
	
	void GSEventSetLocationInWindow(GSEventRef event, CGPoint location);
	void GSEventSetType(GSEventRef event, GSEventType type);
	
	// GSHiccupsEnabled
#pragma mark -
#pragma mark Event queue processing
	
	Boolean GSEventQueueContainsMouseEvent();
	mach_port_t GSGetPurpleApplicationPort() __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_3_0);
	
	Boolean GSGetTimeEventHandling();
	void GSSetTimeEventHandling(Boolean enable);
	void GSSaveEventHandlingTimes();
	
	CFAbsoluteTime _GSEventConvertFromMachTime(uint64_t machTime);
	uint64_t GSCurrentEventTimestamp();
	
	mach_port_name_t GSRegisterPurpleNamedPort(const char* service_name);
	mach_port_name_t GSCopyPurpleNamedPort(const char* service_name);
	mach_port_name_t GSGetPurpleSystemEventPort() __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_3_0);
	
	void GSEventPopRunLoopMode(CFStringRef mode);	///< Stop the event run loop, and remove "mode" from the run loop mode stack if it is at the top.
	void GSEventPushRunLoopMode(CFStringRef mode);	///< Stop the event run loop and push "mode" to the top of run loop mode stack.
	
	void GSEventStopModal();
	void GSEventRunModal(Boolean disallow_restart);
	void GSEventRun();
	
	void GSEventInitialize(Boolean registerPurple);
	
#pragma mark -
#pragma mark Sending events
	void GSSendEvent(const GSEventRecord* record, mach_port_t port);
	void GSSendSimpleEvent(GSEventType type, mach_port_t port) __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_3_0);	///< This calls GSSendEvent with an empty record.
	void GSSendSystemEvent(const GSEventRecord* record);	///< Send event to the PurpleSystemEventPort.	
	
#pragma mark -
#pragma mark Callback functions
	
	/// Only 1 function can be registered.
	
	/// Register a callback function that will be called when PurpleEventCallback() is called.
	void GSEventRegisterEventCallBack(void(*callback)(GSEventRef event));
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	void GSEventRegisterFindWindowCallBack(int(*callback)(CGPoint position));
	void GSEventRegisterTransformToWindowCoordsCallBack(void*) __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_3_0);
#endif
		
#pragma mark -
#pragma mark Touch events
	GSHandInfo GSEventGetHandInfo(GSEventRef event);
	GSPathInfo GSEventGetPathInfoAtIndex(GSEventRef event, CFIndex index);
	void GSEventSetPathInfoAtIndex(GSEventRef event, GSPathInfo pathInfo, CFIndex index);
	
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_1
	void GSEventSetHandInfoScale(GSEventRef event, CGFloat denominator);
#endif
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_0 && __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	void GSEventChangeHandInfoToCancel(GSEventRef event);
#endif
	
	void GSEventDisableHandEventCoalescing(Boolean disableHandCoalescing);
	
	Boolean GSEventIsHandEvent(GSEventRef event);
	Boolean GSEventIsChordingHandEvent(GSEventRef event);
	
	// Always returns 1.
	int GSEventGetClickCount(GSEventRef event);
	
#pragma mark -
#pragma mark Scroll wheel and touch events
	CGFloat GSEventGetDeltaX(GSEventRef event);
	CGFloat GSEventGetDeltaY(GSEventRef event);
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	void GSEventSetDeltaX(GSEventRef event, CGFloat deltaX);
	void GSEventSetDeltaY(GSEventRef event, CGFloat deltaY);
#endif
	
#pragma mark -
#pragma mark Keyboard events
	unsigned short GSEventGetCharacterSet(GSEventRef event);
	GSEventFlags GSEventGetModifierFlags(GSEventRef event);
	Boolean GSEventIsKeyRepeating(GSEventRef event);
	UniChar GSEventGetKeyCode(GSEventRef event);
	
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	GSEventRef _GSCreateSyntheticKeyEvent(UniChar keycode, Boolean isKeyUp, Boolean isKeyRepeating);
#endif
	Boolean GSEventIsKeyCharacterEventType(GSEventRef event, UniChar expected_keycode);
	Boolean GSEventIsTabKeyEvent(GSEventRef event);
	
	CFStringRef GSEventCopyCharactersIgnoringModifiers(GSEventRef event);
	CFStringRef GSEventCopyCharacters(GSEventRef event);
	
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	void _GSPostSyntheticKeyEvent(CFStringRef keys, Boolean isKeyUp, Boolean isKeyRepeating);
#endif
	
#pragma mark -
#pragma mark Accelerometer events
	CGFloat GSEventAccelerometerAxisX(GSEventRef event);
	CGFloat GSEventAccelerometerAxisY(GSEventRef event);
	CGFloat GSEventAccelerometerAxisZ(GSEventRef event);
	
#pragma mark -
#pragma mark Out-of-line data (deprecated)
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	void GSEventRequestOutOfLineData(mach_port_t port, void* unknown);
	mach_msg_return_t GSEventSendOutOfLineData(mach_port_t port, ...);	
#endif
	
#pragma mark -
#pragma mark Hardware manipulation events
	int GSEventDeviceOrientation(GSEventRef event);
	void GSEventRotateSimulator(int x);	// -1 = home button on the left, 0 = portrait, 1 = home button on the right.
	
	void GSEventRestoreSensitivity();
	void GSEventSetSensitivity(int sensitivity);	// or float?
	
	void GSEventLockDevice();	// 1014.

	void GSEventResetIdleTimer();
	void GSEventResetIdleDuration(int a, int b);
	
	void GSEventSetBacklightLevel(float level);
	void GSEventSetBacklightFactor(int factor);
	
#pragma mark -
#pragma mark Application events
	Boolean GSEventIsForceQuitEvent(GSEventRef event);
	void GSEventQuitTopApplication();	// 2010.
	
	void GSSendAppPreferencesChanged(CFStringRef service_name, CFStringRef app_id);
	
	void GSSendApplicationSuspendedSettingsUpdatedEvent(int x, int y, CFStringRef suspendedDefaultPNG, CFStringRef roleID);
	void GSSendApplicationSuspendedEvent(int x, int y, CFStringRef suspendedDefaultPNG, CFStringRef roleID);
	void GSEventFinishedActivating(Boolean b) __OSX_AVAILABLE_STARTING(__MAC_NA,__IPHONE_3_0);
	
	void GSEventSendApplicationOpenURL(CFURLRef url, mach_port_t port);
	
#pragma mark -
#pragma mark Accessory key state events
	GSAccessoryKeyStateInfo GSEventGetAccessoryKeyStateInfo(GSEventRef event);
	GSEventRef GSEventCreateAccessoryKeyStateEvent(GSEventRef event, GSEventFlags flags);
	void GSEventAccessoryKeyStateChanged(unsigned short a, int b, GSEventFlags flags);
	void GSEventAccessoryAvailabilityChanged(unsigned short a, int b);
		
#pragma mark -
#pragma mark Audio events
	SInt32 _GSEventGetSoundActionID(CFStringRef path);
	void _GSEventPlayAlertOrSystemSoundAtPath(CFStringRef path, Boolean loop, Boolean alert);
	
	SInt32 GSEventPrimeSoundAtPath(CFStringRef path);
	void GSEventStopSoundAtPath(CFStringRef path, Boolean unknown);
	void GSEventPlayAlertSoundAtPath(CFStringRef path);// Equivalent to _GSEventPlayAlertOrSystemSoundAtPath(path, 0, 1)
	void GSEventLoopSoundAtPath(CFStringRef path);// Equivalent to _GSEventPlayAlertOrSystemSoundAtPath(path, 1, 0)
	void GSEventPlaySoundAtPath(CFStringRef path);// Equivalent to _GSEventPlayAlertOrSystemSoundAtPath(path, 0, 0)
	
	void GSEventVibrateForDuration(float secs);
	void GSEventStopVibrator();	///< Equivalent to GSEventVibrateForDuration(0)

#pragma mark -
#pragma mark Hardware keyboard events
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_2
	extern const char* kGSEventHardwareKeyboardAvailabilityChangedNotification;	// "GSEventHardwareKeyboardAttached"
	Boolean GSEventIsHardwareKeyboardAttached(void);
	void GSEventSetHardwareKeyboardAttached(Boolean attached);
	
	// "type" must be 10 or 14.
	GSEventRef GSEventCreateKeyEvent(GSEventType type,
									 CGPoint windowLocation,
									 CFStringRef characters,
									 CFStringRef unmodifiedCharacters,
									 GSEventFlags modifiers,
									 uint16_t usagePage,
									 unsigned options7, unsigned options8);
	void GSSendKeyEvent(GSEventType type,
						CGPoint windowLocation,
						CFStringRef characters,
						CFStringRef unmodifiedCharacters,
						GSEventFlags modifiers,
						uint16_t usagePage,
						unsigned short options7,
						unsigned short options8);
	
	uint16_t GSEventGetUsagePage(GSEventRef event);
	void GSEventSetCharCode(GSEventRef event, UniChar charCode);
	void GSEventSetCharacters(GSEventRef event, CFStringRef characters);
	void GSEventSetKeyCode(GSEventRef event, uint16_t keyCode);
	void GSEventSetUnmodifiedCharacters(GSEventRef event, CFStringRef characters);
	
	Boolean GSEventIsHardwareKeyboardEvent(GSEventRef event);
	Boolean GSEventIsKeyVariant(GSEventRef event);
	
#endif
		
	
#if __cplusplus
}
#endif

#endif
