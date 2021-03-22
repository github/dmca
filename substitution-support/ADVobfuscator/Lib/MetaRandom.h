//
//  MetaRandom.h
//  ADVobfuscator
//
// Copyright (c) 2010-2017, Sebastien Andrivet
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

#ifndef MetaRandom_h
#define MetaRandom_h

// Very simple compile-time random numbers generator.

// For a more complete and sophisticated example, see:
// http://www.researchgate.net/profile/Zalan_Szgyi/publication/259005783_Random_number_generator_for_C_template_metaprograms/file/e0b49529b48272c5a6.pdf

#include <random>

namespace andrivet { namespace ADVobfuscator {

namespace
{
    // I use current (compile time) as a seed

    constexpr char time[] = __TIME__; // __TIME__ has the following format: hh:mm:ss in 24-hour time

    // Convert time string (hh:mm:ss) into a number
    constexpr int DigitToInt(char c) { return c - '0'; }
    const int seed = DigitToInt(time[7]) +
                     DigitToInt(time[6]) * 10 +
                     DigitToInt(time[4]) * 60 +
                     DigitToInt(time[3]) * 600 +
                     DigitToInt(time[1]) * 3600 +
                     DigitToInt(time[0]) * 36000;
}

// 1988, Stephen Park and Keith Miller
// "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
// Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimisation:
// with 32-bit math and without division

template<int N>
struct MetaRandomGenerator
{
private:
    static constexpr unsigned a = 16807;        // 7^5
    static constexpr unsigned m = 2147483647;   // 2^31 - 1

    static constexpr unsigned s = MetaRandomGenerator<N - 1>::value;
    static constexpr unsigned lo = a * (s & 0xFFFF);                // Multiply lower 16 bits by 16807
    static constexpr unsigned hi = a * (s >> 16);                   // Multiply higher 16 bits by 16807
    static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);     // Combine lower 15 bits of hi with lo's upper bits
    static constexpr unsigned hi2 = hi >> 15;                       // Discard lower 15 bits of hi
    static constexpr unsigned lo3 = lo2 + hi;

public:
    static constexpr unsigned max = m;
    static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
};

template<>
struct MetaRandomGenerator<0>
{
    static constexpr unsigned value = seed;
};

// Note: A bias is introduced by the modulo operation.
// However, I do belive it is neglictable in this case (M is far lower than 2^31 - 1)

template<int N, int M>
struct MetaRandom
{
    static const int value = MetaRandomGenerator<N + 1>::value % M;
};

}}

#endif
