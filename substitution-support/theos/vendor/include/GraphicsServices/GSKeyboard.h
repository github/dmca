/*
 
GSKeyboard.h ... Hardware keyboard.

Copyright (c) 2010  KennyTM~ <kennytm@gmail.com>
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

#ifndef GSKEYBOARD_H
#define GSKEYBOARD_H

#include <Availability2.h>
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_2

#if __cplusplus
extern "C" {
#endif
	
#include <CoreFoundation/CoreFoundation.h>
#include "GSEvent.h"
	
	enum {
		kUCKeyActionDown = 0,
		kUCKeyActionUp = 1,
		kUCKeyActionAutoKey = 2,
		kUCKeyActionDisplay = 3
	};
		
	typedef struct __GSKeyboard
#if 0
	{
		CFRuntimeBase base;	// 0, 4
		int fildes;
		void* ptr;
		size_t length;
		struct uchrKeyboardLayout* keyLayoutPtr;
		UInt32 keyboardType;
		UInt32 deadKeyState;
		int modifierState;	// 2 = shift, 4 = ctrl, 8 = alt, 16 = cmd
		CFStringRef layoutName;
		pthread_mutex_t mutex;
	}
#endif
	* GSKeyboardRef;
	
	// The keyboard layout list is stored in /System/Library/KeyboardLayouts/USBKeyboardLayouts.bundle/USBKeyboardLayouts.plist
	CFDictionaryRef GSKeyboardHWKeyboardLayoutsPlist(void);
	
	CFTypeID GSKeyboardGetTypeID(void);
	
	GSKeyboardRef GSKeyboardCreate(CFStringRef layoutName, UInt32 keyboardType);
	void GSKeyboardRelease(GSKeyboardRef keyboard);
	
	void GSKeyboardReset(GSKeyboardRef keyboard);
	
	CFStringRef GSKeyboardGetLayout(GSKeyboardRef keyboard);
	void GSKeyboardSetLayout(GSKeyboardRef keyboard, CFStringRef layoutName);
	
	GSEventFlags GSKeyboardGetModifierState(GSKeyboardRef keyboard);
	
	// ref: http://developer.apple.com/mac/library/documentation/Carbon/Reference/Unicode_Utilities_Ref/Reference/reference.html#//apple_ref/doc/uid/TP30000122-CH1g-F10792
	OSStatus GSKeyTranslate(GSKeyboardRef keyboard,
							UInt16 virtualKeyCode, 
							UInt16 keyAction,
							UInt32 modifierKeyState,
							UInt32 keyboardType,
							OptionBits keyTranslateOptions,
							UInt32 *deadKeyState,
							UniCharCount maxStringLength,
							UniCharCount *actualStringLength,
							UniChar unicodeString[]);		   

	OSStatus GSKeyboardTranslateKey(GSKeyboardRef keyboard,
									UInt16 virtualKeyCode,
									UInt16 unknown,
									OptionBits keyTranslateOptions,
									UniCharCount maxStringLength,
									UniCharCount *actualStringLength,
									UniChar unicodeString[],
									UniCharCount *actualStringLength2,
									UniChar unicodeString2[]);
	
#if __cplusplus
}
#endif

#endif

#endif