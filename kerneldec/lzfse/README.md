LZFSE
=====

This is a reference C implementation of the LZFSE compressor introduced in the
[Compression library](https://developer.apple.com/library/mac/documentation/Performance/Reference/Compression/index.html) with OS X 10.11 and iOS 9.

LZFSE is a Lempel-Ziv style data compression algorithm using Finite State Entropy coding.
It targets similar compression rates at higher compression and decompression speed compared to deflate using zlib.

Files
-----
    README.md                             This file ;-)
    Makefile                              Linux / macOS Makefile
    lzfse.xcodeproj                       Xcode project

    src/lzfse.h                           Main LZFSE header
    src/lzfse_tunables.h                  LZFSE encoder configuration
    src/lzfse_internal.h                  LZFSE internal header
    src/lzfse_decode.c                    LZFSE decoder API entry point
    src/lzfse_encode.c                    LZFSE encoder API entry point
    src/lzfse_decode_base.c               LZFSE decoder internal functions
    src/lzfse_encode_base.c               LZFSE encoder internal functions
    src/lzfse_encode_tables.h             LZFSE encoder tables
    
    src/lzfse_fse.h                       FSE entropy encoder/decoder header
    src/lzfse_fse.c                       FSE entropy encoder/decoder functions
    
    src/lzvn_decode_base.h                LZVN decoder
    src/lzvn_decode_base.c
    src/lzvn_encode_base.h                LZVN encoder
    src/lzvn_encode_base.c
    
    src/lzfse_main.c                      Command line tool

Building on OS X
----------------

    $ xcodebuild install DSTROOT=/tmp/lzfse.dst

Produces the following files in `/tmp/lzfse.dst`:

    usr/local/bin/lzfse                   command line tool
    usr/local/include/lzfse.h             LZFSE library header
    usr/local/lib/liblzfse.a              LZFSE library

Building on Linux
-----------------

Tested on Ubuntu 15.10 with gcc 5.2.1 and clang 3.6.2. Should work on any recent distribution.

    $ make install INSTALL_PREFIX=/tmp/lzfse.dst/usr/local

Produces the following files in `/tmp/lzfse.dst`:

    usr/local/bin/lzfse                   command line tool
    usr/local/include/lzfse.h             LZFSE library header
    usr/local/lib/liblzfse.a              LZFSE library

Building with cmake
-------------------

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make install

Installs the header, library, and command line tool in `/usr/local`.

Bindings
--------

Python: [dimkr/pylzfse](https://github.com/dimkr/pylzfse)

