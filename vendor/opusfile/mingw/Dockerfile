FROM fedora:32
MAINTAINER opus@xiph.org

# Linux build.
RUN dnf update -y --setopt=deltarpm=0
RUN dnf install -y git gcc make wget xz
RUN dnf install -y autoconf automake libtool pkgconfig

# mingw cross build.
RUN dnf install -y mingw32-gcc zip

RUN dnf clean all

RUN git clone https://gitlab.xiph.org/xiph/opusfile.git

WORKDIR opusfile
RUN git pull
COPY Makefile mingw/Makefile
RUN make -C mingw
RUN ./autogen.sh && ./configure --host=i686-w64-mingw32 --prefix=${PWD}/mingw PKG_CONFIG_PATH=${PWD}/mingw/lib/pkgconfig && make && make check && make install
RUN make -C mingw package
