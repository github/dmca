# libplist

## About

A small portable C library to handle Apple Property List files in binary or XML.

## Requirements

Software:
* make
* autoheader
* automake
* autoconf
* libtool
* pkg-config
* gcc or clang

Optional:
* cython (Python bindings)
* doxygen (Documentation)

## Installation

To compile run:
```bash
./autogen.sh
make
sudo make install
```

If you require a custom prefix or other option being passed to `./configure`
you can pass them directly to `./autogen.sh` like this:
```bash
./autogen.sh --prefix=/opt/local --without-cython
make
sudo make install
```

## Who/What/Where?

* Home: https://www.libimobiledevice.org/
* Code: `git clone https://git.libimobiledevice.org/libplist.git`
* Code (Mirror): `git clone https://github.com/libimobiledevice/libplist.git`
* Tickets: https://github.com/libimobiledevice/libplist/issues
* Mailing List: https://lists.libimobiledevice.org/mailman/listinfo/libimobiledevice-devel
* IRC: irc://irc.freenode.net#libimobiledevice
* Twitter: https://twitter.com/libimobiledev

## Credits

Apple, iPhone, iPod, and iPod Touch are trademarks of Apple Inc.
libimobiledevice is an independent software library and has not been
authorized, sponsored, or otherwise approved by Apple Inc.

README Updated on: 2019-05-16
