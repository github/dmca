//
//  MetaString1.h
//  ADVobfuscator
//
// Copyright (c) 2010-2014, Sebastien Andrivet
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Get latest version on https://github.com/andrivet/ADVobfuscator

#ifndef MetaString1_h
#define MetaString1_h

#include "Inline.h"

// First implementation of an obfuscated string
// Limitations:
// - Truncate strings with more than 6 characters
// - May generate strange things if string has less than 6 characters
// - Hard-coded key
// - Hard-coded algorithm

namespace andrivet { namespace ADVobfuscator {

// Represents an obfuscated string
// Almost everything is compile-time
// Except decrypt()
    
template<int... I> // I is a list of indexes 0, 1, 2, ...
struct MetaString1
{
    // Constructor. Evaluated at compile time
    constexpr ALWAYS_INLINE MetaString1(const char* str)
    : buffer_ {encrypt(str[I])...} { }
    
    // Runtime decryption. Most of the time, inlined
    inline const char* decrypt()
    {
        for(size_t i = 0; i < sizeof...(I); ++i)
            buffer_[i] = decrypt(buffer_[i]);
        buffer_[sizeof...(I)] = 0;
        return buffer_;
    }

private:
    // Encrypt / decrypt a character of the original string
    constexpr char encrypt(char c) const { return c ^ 0x55; }
    constexpr char decrypt(char c) const { return encrypt(c); }
    
private:
    // Buffer to store the encrypted string + terminating null byte
    char buffer_[sizeof...(I) + 1];
};

// Custom literal suffix. Not supported by Intel compiler
#if !defined(__ICC) && !defined(__INTEL_COMPILER)
inline const char* operator "" _obfuscated1(const char* str, size_t)
{
    return MetaString1<0, 1, 2, 3, 4, 5>(str).decrypt();
}
#endif
    
}}

// Alternative (prefix) notation
#define OBFUSCATED1(str) (MetaString1<0, 1, 2, 3, 4, 5>(str).decrypt())

#endif
