/*
 
substrate2.h ... Convenient wrapper for MobileSubstrate.

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

#ifndef SUBSTRATE2_H
#define SUBSTRATE2_H

// Use substrate.h on iPhoneOS, and APELite on x86/ppc for debugging.
#ifdef __arm__
#import <substrate.h>
#define REGPARM3 
#elif __i386__ || __ppc__
#import <objc/runtime.h>
#import <objc/message.h>
extern
#if __cplusplus
"C"
#endif
void* APEPatchCreate(const void* original, const void* replacement);
#define MSHookFunction(original, replacement, result) (*(result) = APEPatchCreate((original), (replacement)))
#define MSHookMessageEx(class, selector, replacement, result) (*(result) = method_setImplementation(class_getInstanceMethod((class), (selector)), (replacement)))
#define REGPARM3 __attribute__((regparm(3)))
#else
#error Not supported in non-ARM/i386/PPC system.
#endif

#define Original(funcname) original_##funcname

#define DefineHook(rettype, funcname, ...) \
rettype funcname (__VA_ARGS__); \
static rettype (*original_##funcname) (__VA_ARGS__); \
static rettype replaced_##funcname (__VA_ARGS__)

#define DefineObjCHook(rettype, funcname, ...) \
static rettype (*original_##funcname) (__VA_ARGS__); \
static rettype replaced_##funcname (__VA_ARGS__)

#define DefineHiddenHook(rettype, funcname, ...) \
static rettype (*funcname) (__VA_ARGS__); \
REGPARM3 static rettype (*original_##funcname) (__VA_ARGS__); \
REGPARM3 static rettype replaced_##funcname (__VA_ARGS__)

#define InstallHook(funcname) MSHookFunction(funcname, replaced_##funcname, (void**)&original_##funcname)
#define InstallObjCInstanceHook(cls, sel, delimited_name) MSHookMessageEx((cls), (sel), (IMP)replaced_##delimited_name, (IMP*)&original_##delimited_name)
#define InstallObjCClassHook(cls, sel, delimited_name) MSHookMessageEx(object_getClass(cls), (sel), (IMP)replaced_##delimited_name, (IMP*)&original_##delimited_name)

#endif
