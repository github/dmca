What is ADVobfuscator?
======================

**ADVobfuscator** demonstates how to use `C++11/14` language to generate, at compile time, obfuscated code without using any external tool and without modifying the compiler. The technics presented rely only on `C++11/14`, as standardized by ISO. It shows also how to introduce some form of randomness to generate polymorphic code and it gives some concrete examples like the encryption of strings literals and the obfuscation of calls using finite state machines.

**ADVobfuscator** is delivered as a set of source file (source code library). You have to include them into your project and call the appropriate functions or macro. This is not a magic black box or some kind of code pre-processor or post-processor. It has to be integrated manually into your source code.

If you are looking for a more transparent obfuscator, look at [Obfuscator-LLVM](https://github.com/obfuscator-llvm/obfuscator/wiki) or [strong.codes](https://strong.codes) (commercial version of Obfuscator-LLVM). *Note: I have no affiliation with Obfuscator-LLVM and strong.codes. I just know some of those swiss folks.*


News
====

August 2017
------------

- Enhance the compatibility with compiler's optimizer (such as `GCC` with `O3`). Previously, the code was sometimes overoptimized by the compiler and as a consequence, non-obfuscated strings remains in the binary code. This was more or less due to a bug in **ADVobfuscator** (I was not using `volatile`).
- Fix a stupid mistake in algorithm #2 (shift). Sometimes, the encryption key is 0.
- Clearly seperate the library code (under `Lib`) from the examples (under `Examples` and `DocCode`).
- Check the compatibility with `C++14`. In the future, I will probably drop the support of `C++11` and keep only `C++14` and `C++17` compatibility.

August 2016
-----------

Several enhancement (suppress warnings, fix some errors, enhance portability). In more details:

- Increase the warning level for Visual Studio and GCC
- Remove several (all) compilation warnings
- Replace `int` by `size_t` when it is appropriate (`sizeof`, `position`)
- Remove unused parameters
- Keys are now of type `char` (were of type `int`, it was wrong)
- Replace the non-portable `##__VA_ARGS__` by the portable `__VA_ARGS__`. As a consequence, new macros (ending with `0`) are defined when there are no parameters
- Remove some (stupid) syntax errors (`;` at the end of some functions). C/C++ is odd: you have to put a `;` when you define a `struct`, but none when you define a function
- A integral type is computed at compile time to store pointers to functions

August 2015
-----------

**ADVobfuscator** code has been updated for Visual Studio 2015. **ADVobfuscator** is now compatible with the RTM release of Visual Studio 2015 (previous versions or CTP releases are not). The whitepaper is not yet updated. The code has also been modified in order to avoid problems with `O3` optimization and `GCC`. `GCC` with `O3` defeats obfuscation (because it optimizes too much) and sometimes generates wrong code (not clear yet if it is a bug in `GCC` or in **ADVobfuscator**).


How to use it?
==============

First you have to follow the requirements below. Then, you just have to include **ADVobfuscator** header files and use macros such as `OBFUSCATED` to protect your strings or function calls.

Look at the examples in the `Examples` folder.

Prerequisites
--------------

* A `C++11` or `C++14` compatible compiler (i.e. a compiler that is not too old)
* **Obfuscated strings**: no other prerequisite
* **Obfuscated calls and predicates**: You have to install the [Boost library](www.boost.org). See below

Boost Library
-------------

You have to install the [Boost library](www.boost.org) in order to use some features of **ADVdetector** (it is used by FSM). To install `Boost`:

- Debian / Ubuntu: `sudo apt-get install libboost-all-dev`
- Mac OS X: `brew install boost`
- Windows: [Download Boost](http://www.boost.org/users/download/) and install it. Then you have to change the Visual Studio project to point to Boost

Examples
--------

### Linux

```
cd Examples
make
```

Each example is in its subdirectory.

### macOS

Open `ADVobfuscator.xcworkspace` and build each example.

### Windows

Open `ADVobfuscator.sln`. Be sure to change the path to `Boost` library for each project (Properties | C/C++ | General | Additional Include Directories).


Debug Builds
-------------

Debug builds are very special: Compiler do not have (and do not most of the time) respect statement such as `inline` or `constexpr`. All optimizations are also, by default, disabled. Compilers are doing this for a good reason: let you debug, single step, etc.

As a consequence, **ADVobfuscator** is **not** compatible with Debug builds. It works only for Release builds.

Compatibility
=============

**ADVobfuscator** has been tested with:

* Xcode (LLVM) 8.1.0 under Mac OS X 10.12
* GCC 7.2.0 under Debian 10 (buster - testing)
* Visual Studio 2017 (15.3.3) under Windows 10
* Boost 1.65.0

Other compilers are probably compatible if they are `C++11/14` compliant.


Future Directions
=================

These are ideas for the next few months:

- Create version 5 of `MetaString`: instead of choosing one algorithm, it will combine several. This way, tools such as [XOR breakers](https://digital-forensics.sans.org/blog/2013/05/14/tools-for-examining-xor-obfuscation-for-malware-analysis) will not work.
- Switch to `C++14` and `C++17`. It will simplify some parts of the code and `C++` is now well supported by compilers such as `GCC` and `CLANG`
- Try to better support Visual Studio 2015, but only if it does not implies a restructuring of the code
- Introduce **White Box Cryptography** (AES in particular). Still in my mind but I am not very active for the moment
- Is my code compatible with ASLR? In fact, I don't konw and I have to solve this (I need it for another project)
- Apply the techniques used in `MetaString4.h` into `ObfuscatedCall.h`
- )ntroduce Unit Testing


Files and Folders
=================

| Files and Folders               | Description |
| ------------------------------- |-------------|
| `README.md` | This file |
| `Lib` | **ADVobfuscator** library |
| `Examples` | Examples of using **ADVobfuscator** |
| `Examples/Makefile` | Make file that build all the examples |
| `Examples/ObfuscatedString` | Example of using **ADVobfuscator** to obfuscate strings |
| `Examples/ObfuscatedCalls` | Example of using **ADVobfuscator** to obfuscate function calls |
| `Examples/DetectDebugger` | Example of using **ADVobfuscator** to obfuscate function calls triggered by a predicate  |
| `Docs` | My talks and white papers |
| `DocCode` | Code memtionned in the documents |

### Lib

| Files                           | Description |
| ------------------------------- |-------------|
| `Indexes.h`                     | Generate list of indexes at compile time (0, 1, 2, ... N) |
| `MetaFSM.h`                     | Template to generate Finite State Machines at compile time |
| `MetaRandom.h`                  | Generate a pseudo-random number at compile time |
| `MetaString.h`                  | Obfuscated string - version 4 - Random encryption algorithm |
| `ObfuscatedCall.h`              | Obfuscate function call |
| `ObfuscatedCallWithPredicate.h` | Obfuscate function call, execute a FSM based on a predicate |
| `ADVobfuscator.xcodeproj`       | Project for Apple Xcode |
| `ADVobfuscator.sln`             | Visual Studio 2015 Solution |

### DocCode

| Files                           | Description |
| ------------------------------- |-------------|
| `MetaFactorial.h`               | Compute factorial at compile time |
| `MetaFibonacci.h`               | Compute fibonacci sequence at compile time |
| `MetaString1.h`                 | Obfuscated string - version 1 |
| `MetaString2.h`                 | Obfuscated string - version 2 - Remove truncation |
| `MetaString3.h`                 | Obfuscated string - version 3 - Random key |
| `MetaString4.h`                 | Obfuscated string - version 4 - Random encryption algorithm |

### Examples

| Files                           | Description |
| ------------------------------- |-------------|
| `DetectDebugger.cpp`            | Debugger detection, implemented for Mac OS X and iOS. It is used by `ObfuscatedCallWithPredicate` (FSM) |
| `DetectDebugger.h`              | Debugger detection, declaration |
| `main.cpp`                      | Samples |
| `Makefile`                      | Simple makefile for `GCC` |
| `ADVobfuscator.sln`             | Visual Studio 2017 Solution |
| `ADVobfuscator.xcworkspace`     | Xcode workspace |


Copyright and license
=====================

Written by Sebastien Andrivet - Copyright &copy; 2010-2017 Sebastien Andrivet.

> Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
>
> 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
> 
> 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
> 
> 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

> THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
