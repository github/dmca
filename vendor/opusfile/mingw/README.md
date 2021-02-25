# Cross-compiling under mingw

Just running `make libopusfile-0.dll` in this directory should download
and build opusfile and its dependencies. Some mingw
libraries need to be compiled into the final package.

## Generic instructions

To build opusfile under mingw, you need to first build:

- libogg
- libopus
- openssl

For 'make check' to work, you may need wine installed.

To build openssl, try:

 CROSS_COMPILE="i686-w64-mingw32-" ./Configure mingw no-asm no-shared --prefix=$PWD/mingw && make depend && make -j8 && make install

To build opusfile, try:

 CC=i686-w64-mingw32-gcc PKG_CONFIG_PATH=$PWD/lib/pkgconfig RANLIB=i686-w64-mingw32-ranlib make -f ../unix/Makefile

## Building the release package

Running `make package` should produce a binary package.

The steps are something like

- Compile dynamic opusfile with:
  - ./configure --host=i686-w64-mingw32 --prefix=/path/to/builddir/mingw \
        PKG_CONFIG_PATH=/path/to/builddir/mingw/lib/pkgconfig
  - make && make check && make -C doc/latex
    - If Doxygen fails because of unescaped '#' characters in URLs
      Update to at least Doxygen 1.8.15. Doxygen 1.8.3 also works.
- mkdir opusfile-${version}-win32
- Copy AUTHORS COPYING README.md include/opusfile.h to the release dir.
  - Don't put opusfile.h in an opusfile-${version}-win32/include directory,
    just put it straight in the release dir.
- Merge changes between README.md and the version in the last
  binary release. E.g. it's good to include versions of the dependencies,
  release notes, etc.
- Convert README.md to DOS line endings.
- Copy .libs/libopusfile-0.dll to the release dir.
- Copy .libs/libopusfile.a to the release dir.
- Copy .libs/libopusurl-0.dll to the release dir.
- Copy .libs/libopusurl.a to the release dir.
- Copy mingw/bin/*.dll to the release dir for dependencies.
- Copy any other dependent dlls, e.g. on Fedora 32 I needed to copy
    /usr/i686-w64-mingw32/sys-root/mingw/bin/libgcc_s_dw2-1.dll
    /usr/i686-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll
  On Fedora 23 I needed to copy
    /usr/i686-w64-mingw32/sys-root/mingw/bin/libgcc_s_sjlj-1.dll
    /usr/i686-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll
  On Gentoo I needed to copy
    /usr/lib64/gcc/i686-w64-mingw32/7.3.0/libgcc_s_sjlj-1.dll
  TODO: It may be possible to avoid this with CFLAGS="-static-libgcc"
- Copy doc/latex/refman.pdf to opusfile-${version}-win32/opusfile-${version}.pdf
- Copy examples/.libs/*.exe to the release dir.
- Run "i686-w64-ming32-strip *.dll *.a *.exe" in the release dir.
- In the release dir, run:
    sha256sum * > SHA256SUMS.txt
    gpg --detach-sign --armor SHA256SUMS.txt
- In the parent directory, create the archive:
    zip -r opusfile-${version}-win32.zip opusfile-${version}-win32/*
- Copy the archive to a clean system and verify the examples work
  to make sure you've included all the necessary libraries.

