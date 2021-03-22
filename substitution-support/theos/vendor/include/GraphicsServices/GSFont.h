/*

GSFont.h ... Graphics Service Font
 
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

#ifndef GSFONT_H
#define GSFONT_H

#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Availability2.h>

#if __cplusplus
extern "C" {
#endif
	
	/*!
	 @file GSFont.h
	 @brief Mid-level wrapper of font stuff.
	 @author Kenny TM~
	 @date 2009 Aug 5
	 
	 GSFont is a "mid-level" wrapper of many CGFont operations. It includes functions that
	  - Caching common fonts, and
	  - Creating common fonts by a font family, traits and size.
	 */
	
	
	typedef struct __GSFont* GSFontRef;
	
	typedef enum __GSFontTraitMask {
		// Why not kGS...? Ask Apple. http://www.opensource.apple.com/source/WebCore/WebCore-351.9/platform/graphics/mac/FontCacheMac.mm
		GSItalicFontMask = 1,
		GSBoldFontMask = 2,
		// Backward compatibility with WinterBoard.
		kGSFontTraitNone = 0,
		kGSFontTraitBold = 1,
		kGSFontTraitItalic = 2,
		kGSFontTraitBoldItalic = 3
	} GSFontTraitMask;
	
	// Backward compatibility with WinterBoard.
	typedef GSFontTraitMask GSFontTrait;
	 
	/*
	 struct __GSFont {
		 __CFRuntimeBase _base;	// 0
		 CGFontRef _cgFont;		// 8
		 CGFloat _size;	// c
		 struct GSFontTraits _traits;	// 10
		 struct GSFontTraits _xorTraits;	// 14
		 CGFloat _ascent;	// 18
		 CGFloat _descent;	// 1c
		 CGFloat _lineSpacing;	// 20
		 CGFloat _lineGap;	// 24
		CGFloat _maximumAdvanceWidth // 28 (since 3.2)
	 }
	 */
	
	CFHashCode GSFontHash(GSFontRef font);	///< Compute the font's hash value.
	Boolean GSFontEqual(GSFontRef a, GSFontRef b);	///< Determine if two fonts are equal.
	CFTypeID GSFontGetTypeID();	///< Get the CoreFoundation type identifier of GSFont.
	
	/*!
	 @brief Set to use legacy font metrics or not.
	 
	 The setting will apply when you create a GSFont. The difference between using legacy metrics or not is that, with
	 the modern metrics, most floating point operations are single precision, and float-to-int operations are rounded up
	 (ceiling), while for legacy metrics there are more double precision operations and floating points are rounded to the
	 nearest integers (floor(0.5+x)). There may be some other differences in the algorithm.
	 */
	void GSFontSetUseLegacyFontMetrics(Boolean useLegacyFontMetrics);
	Boolean GSFontGetUseLegacyFontMetrics();	///< Get whether legacy font metrics is used.
	
	CGFontRef GSFontGetCGFont(GSFontRef font);	///< Get CGFont from a GSFont.
	Boolean GSFontIsBold(GSFontRef font);	///< Check whether a font is bold.
	GSFontTraitMask GSFontGetTraits(GSFontRef font);	///< Get the traits mask of the font.
	GSFontTraitMask GSFontGetSynthesizedTraits(GSFontRef font);	///< Get the synthesized traits mask of the font.
	CGFloat GSFontGetSize(GSFontRef font);	///< Get the font's point size
	CGFloat GSFontGetAscent(GSFontRef font);	///< Get the font's ascent.
	CGFloat GSFontGetDescent(GSFontRef font);	///< Get the font's descent.
	CGFloat GSFontGetLineSpacing(GSFontRef font);	///< Get the font's line spacing.
	CGFloat GSFontGetLineGap(GSFontRef font);	///< Get the font's line gap.
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_2
	CGFloat GSFontGetMaximumAdvanceWidth(GSFontRef font);	///< Get the font's maximum advance width.
#endif
	
	typedef enum CGFontRenderingMode {
		kCGFontRenderingModeAntialiased = 3
	} CGFontRenderingMode;
	
	/// Compute the transformed advances (occupied size?) of glyphs in the font.
	Boolean GSFontGetGlyphTransformedAdvances(GSFontRef font, const CGAffineTransform transforms[], CGFontRenderingMode mode, const CGGlyph glyph[], unsigned count, CGSize transformedAdvances[]);
	
	/// Find the glyphs of the corresponding Unicode characters.
	void GSFontGetGlyphsForUnichars(GSFontRef font, const UniChar unichars[], CGGlyph glyphs[], size_t count);
	size_t GSFontGetNumberOfGlyphs(GSFontRef font);	///< Get the number of glyphs in the font.
	
	int GSFontGetUnitsPerEm(GSFontRef font);
	CGFloat GSFontGetCapHeight(GSFontRef font);
	CGFloat GSFontGetXHeight(GSFontRef font);
	
	GSFontTraitMask GSFontGetTraitsForName(const char* fontName);
	Boolean GSFontIsFixedPitch(GSFontRef font);
	
	const char* GSFontGetFamilyName(GSFontRef font);
	const char* GSFontGetFullName(GSFontRef font);
	CFArrayRef GSFontCopyFontNamesForFamilyName(const char* familyName);	// an array of CFString, not const char*
	CFArrayRef GSFontCopyFamilyNames();
	
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_3_2
	void GSFontPurgeFontCache();
#endif
	void GSFontInitialize();
	
	/// Use the specified font in the context.
	void GSFontSetFont(CGContextRef context, GSFontRef font);
	
	/// Add a CGFont for search.
	Boolean GSFontAddCGFont(CGFontRef cgFont);
	/// Add a font from a file to search.
	Boolean GSFontAddFromFile(const char* filename);
	
	GSFontRef GSFontCreateWithName(const char* fontName, GSFontTraitMask traits, CGFloat fontSize);
	const char* GSFontGetPostScriptName(GSFontRef font);
	
#if __cplusplus
}
#endif

#endif
