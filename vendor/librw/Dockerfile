FROM ubuntu:rolling

ENV PS2DEV /ps2dev
ENV PS2SDK $PS2DEV/ps2sdk
ENV PATH   $PATH:$PS2DEV/bin:$PS2DEV/ee/bin:$PS2DEV/iop/bin:$PS2DEV/dvp/bin:$PS2SDK/bin

ENV DEBIAN_FRONTEND noninteractive

ENV TOOLCHAIN_GIT_URL git://github.com/ps2dev/ps2toolchain.git
ENV TOOLCHAIN_GIT_BRANCH master

ENV PREMAKE5_URL=https://github.com/premake/premake-core/releases/download/v5.0.0-alpha12/premake-5.0.0-alpha12-linux.tar.gz

RUN mkdir -p "$PS2DEV" "$PS2SDK" \
    && apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y \
        build-essential \
        cmake \
        autoconf \
        bzip2 \
        gcc \
        git \
        libucl-dev \
        make \
        patch \
        vim \
        wget \
        zip \
        zlib1g-dev \
        libglfw3-dev \
        libsdl2-dev \
    && git clone -b $TOOLCHAIN_GIT_BRANCH $TOOLCHAIN_GIT_URL /toolchain \
    && cd /toolchain \
    && ./toolchain.sh \
    && git clone git://github.com/ps2dev/ps2eth.git /ps2dev/ps2eth \
    && make -C /ps2dev/ps2eth \
    && git clone git://github.com/ps2dev/ps2-packer.git /ps2-packer \
    && make install -C /ps2-packer \
    && rm -rf \
        /ps2-packer \
        /ps2dev/ps2eth/.git \
        /ps2dev/ps2sdk/test.tmp \
        /ps2dev/test.tmp \
        /toolchain \
    && rm -rf /var/lib/apt/lists/* \
    && wget "$PREMAKE5_URL" -O /tmp/premake5.tar.gz \
    && tar xf /tmp/premake5.tar.gz -C /usr/bin/ \
    && rm /tmp/premake5.tar.gz \
    && groupadd 1000 -g 1000 \
    && groupadd 1001 -g 1001 \
    && groupadd 2000 -g 2000 \
    && groupadd 999 -g 999 \
    && useradd -ms /bin/bash builder -g 1001 -G 1000,2000,999 \
    && printf "builder:builder" | chpasswd \
    && adduser builder sudo \
    && printf "builder ALL= NOPASSWD: ALL\\n" >> /etc/sudoers

USER builder
WORKDIR /home/builder
