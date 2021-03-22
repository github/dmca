/*

GSColor.h ... Graphics Services Color.
 
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

#ifndef GSCOLOR_H
#define GSCOLOR_H

#include <CoreGraphics/CoreGraphics.h>

/*!
 @file GSColor.h
 @brief Mid-level wrapper of color stuff.
 @author Kenny TM~
 @date 2009 Aug 5
 
 GSColor is a "mid-level" wrapper of some CGColor operations. It includes functions that
  - Obtain the RGBA components of a color,
  - Caching common colors, and
  - Blending two colors together.
 
 */

#if __cplusplus
extern "C" {
#endif

	// Returns the gAdditionalSystemColorTable.
	// void _GSColorRegisterAdditionalSystemColors(?);
	
	/// Set both stroke and fill color of the context to the specified color.
	void GSColorSetColor(CGContextRef context, CGColorRef color);
	
	/// Get the RGBA components (in [0, 1]) of the color.
	void GSColorGetRGBAComponents(CGColorRef color, CGFloat* r, CGFloat* g, CGFloat* b, CGFloat* a);
	
	CGFloat GSColorGetAlphaComponent(CGColorRef color);	///< Get the alpha component.
	CGFloat GSColorGetBlueComponent(CGColorRef color);	///< Get the blue component.
	CGFloat GSColorGetGreenComponent(CGColorRef color);	///< Get the green component.
	CGFloat GSColorGetRedComponent(CGColorRef color);	///< Get the red component.
	
	typedef enum GSSystemColor {
		kGSBlackColor,
		kGSWhiteColor,
		kGSGrayColor,	// 50%
		kGSLightGrayColor,	// 70% 
		kGSDarkGrayColor,	// 30%
		kGSRedColor,
		kGSGreenColor,
		kGSBlueColor,
		kGSCyanColor,
		kGSMagentaColor,
		kGSYellowColor,
		kGSOrangeColor,	// #FF8000
		kGSPurpleColor,	// #800080
		kGSBrownColor,	// #996633
		kGSClearColor
	} GSSystemColor;
	
	/// Get a "system color". The color will be cached by the system.
	CGColorRef GSColorForSystemColor(GSSystemColor colorType);
	
	CGColorRef GSColorGetShadowColor();	///< Get the shadow color, i.e. black.
	CGColorRef GSColorGetHighlightColor();	///< Get the highlight color, i.e. white.
	
	/// Set the fill and stroke color of the context to the specified system color.
	void GSColorSetSystemColor(CGContextRef context, GSSystemColor colorType);
	
	/// Create a CGColor. Additionally, if the colorSpace is Device Gray or Device RGB, the color will be cached.
	CGColorRef GSColorCreate(CGColorSpaceRef colorSpace, CGFloat components[]);
	
	/// Create a grayscale cached CGColor.
	CGColorRef GSColorCreateWithDeviceWhite(CGFloat white, CGFloat alpha);
	
	/// Create an RGBA cached CGColor.
	CGColorRef GSColorCreateColorWithDeviceRGBA(CGFloat r, CGFloat g, CGFloat b, CGFloat a);
	
	/// Create a new color by blending two colors together.
	/// The resulting color = first * (1 - fraction) + second * fraction.
	CGColorRef GSColorCreateBlendedColorWithFraction(CGColorRef first, CGColorRef second, CGFloat fraction);
	
	/// Create a shadowed color. Equivalent to GSColorCreateBlendedColorWithFraction(color, GSColorGetShadowColor(), level).
	CGColorRef GSColorCreateShadowWithLevel(CGColorRef color, CGFloat level);
	
	/// Create a highlighted color. Equivalent to GSColorCreateBlendedColorWithFraction(color, GSColorGetHighlightColor(), level).
	CGColorRef GSColorCreateHighlightWithLevel(CGColorRef color, CGFloat level);
	
#if __cplusplus
}
#endif

#endif