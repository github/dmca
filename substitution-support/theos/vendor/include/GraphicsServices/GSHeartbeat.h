/*
 
FILE_NAME ... FILE_DESCRIPTION

Copyright (c) 2009  KennyTM~ <kennytm@gmail.com>
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

#ifndef GSHEARTBEAT_H
#define GSHEARTBEAT_H

#include <CoreGraphics/CoreGraphics.h>

#if __cplusplus
extern "C" {
#endif	
	
	/*!
	 @file GSHeartbeat.h
	 @brief Calls function when display updates
	 @author Kenny TM~
	 @date 2009 Sept 24
	 
	 GSHeartbeat is an API that allows you to register a callback function when the display is updated. It has the same
	 purpose as CADisplayLink, but have different origin.
	 
	 */	
	
	typedef struct __GSHeartbeat* GSHeartbeatRef;
	typedef double GSHeartbeatTime;
	typedef void (*GSHeartbeatCallback)(void* context, GSHeartbeatTime time);
	
	/*
	 struct __GSHeartbeat {
		 arg0 // 8
		 arg3 // c
		 Boolean isPaused;	// 10
		 CFRunLoopRef runloop // 14
		 CFStringRef mode; // 18
		 void* framebuffer; // 1c
		 CFRunLoopSourceRef source;	// 20
	 }
	 */
	

	GSHeartbeatTime GSHeartbeatGetCurrentTime();
	
	CFTypeID GSHeartbeatGetTypeID();
	
	GSHeartbeatRef GSHeartbeatCreate(GSHeartbeatCallback callback, CFStringRef runloopMode, Boolean useTVOut, void* context);
	
	Boolean GSHeartbeatIsPaused(GSHeartbeatRef heartbeat);
	void GSHeartbeatPause(GSHeartbeatRef heartbeat, Boolean pause_on);
	
	void GSHeartbeatSetRunLoopMode(GSHeartbeatRef heartbeat, CFStringRef mode);
	void GSHeartbeatTickle(GSHeartbeatRef heartbeat, CFRunLoopRef runloop);
	
	void GSHeartbeatInvalidate(GSHeartbeatRef heartbeat);
	
#if __cplusplus
}
#endif

#endif
