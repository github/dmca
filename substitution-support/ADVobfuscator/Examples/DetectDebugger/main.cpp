//
//  main.cpp
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

// To remove Boost assert messages
#if !defined(DEBUG) || DEBUG == 0
#define BOOST_DISABLE_ASSERTS
#endif

#pragma warning(disable: 4503)

#include <iostream>
#include "MetaString.h"
#include "DetectDebugger.h"
#include "ObfuscatedCallWithPredicate.h"

using namespace std;
using namespace andrivet::ADVobfuscator;


void ImportantFunctionInTheApplication()
{
    cout << OBFUSCATED("PRISM") << endl;
}

// Predicate
struct DetectDebugger { bool operator()() { return AmIBeingDebugged(); } };

// Obfuscate functions calls. Behaviour is dependent of a runtime value (debugger detected or not)
// The function 'ImportantFunctionInTheApplication' will only be called if a debugger is NOT detected
void SampleFiniteStateMachine()
{
    OBFUSCATED_CALL_P0(DetectDebugger, ImportantFunctionInTheApplication);
}

// Entry point
int main(int, const char *[])
{
    SampleFiniteStateMachine();
    return 0;
}
