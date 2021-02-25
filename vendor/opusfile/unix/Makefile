# NOTE: This Makefile requires GNU make
# Location to put the targets.
TARGETBINDIR = .
TESTBINDIR = tests
TARGETLIBDIR = .
# Name of the targets
LIBOPUSFILE_TARGET = libopusfile.a
LIBOPUSURL_TARGET = libopusurl.a
OPUSFILE_EXAMPLE_TARGET = opusfile_example
SEEKING_EXAMPLE_TARGET = seeking_example
# Test targets
#TODO: tests
FOO_TARGET = foo
# The command to use to generate dependency information
MAKEDEPEND = ${CC} -MM
#MAKEDEPEND = makedepend -f- -Y --
# Optional features to enable
#CFLAGS := $(CFLAGS) -DOP_HAVE_LRINTF
CFLAGS := $(CFLAGS) -DOP_ENABLE_HTTP
# Extra compilation flags.
# You may get speed increases by including flags such as -O2 or -O3 or
#  -ffast-math, or additional flags, depending on your system and compiler.
# The -g flag will generally include debugging information.
CFLAGS := -g $(CFLAGS)
CFLAGS := -DOP_ENABLE_ASSERTIONS $(CFLAGS)
# These are gcc-only, but not actually critical.
CFLAGS := -fPIC $(CFLAGS)
CFLAGS := -std=c89 -pedantic $(CFLAGS)
CFLAGS := -fvisibility=hidden $(CFLAGS)
CFLAGS := -Wextra -Wno-parentheses -Wno-long-long $(CFLAGS)
CFLAGS := -Wall $(CFLAGS)
# The list of pkg-config packages we depend on.
PACKAGES := ogg opus
ifeq ($(findstring -DOP_ENABLE_HTTP,${CFLAGS}),-DOP_ENABLE_HTTP)
PACKAGES += openssl
endif
# The location of include files.
# Modify these to point to your Ogg and Opus include directories if they are
#  not installed in a standard location.
CINCLUDE := `pkg-config --cflags ${PACKAGES}`

# Libraries to link with, and the location of library files.
LIBS := `pkg-config --libs ${PACKAGES}`
ifeq ($(findstring -DOP_HAVE_LRINTF,${CFLAGS}),-DOP_HAVE_LRINTF)
LIBS := -lm $(LIBS)
endif

# Extras for the MS target
ifneq ($(findstring mingw,${CC}),)
LIBS += -lwsock32 -lws2_32 -lgdi32 -lcrypt32
EXEEXT := .exe
endif

RANLIB ?= ranlib

#TODO: tests
FOO_LIBS =

# ANYTHING BELOW THIS LINE PROBABLY DOES NOT NEED EDITING
CINCLUDE := -I../include ${CINCLUDE}
LIBSRCDIR = ../src
BINSRCDIR = ../examples
TESTSRCDIR = ${LIBSRCDIR}
WORKDIR = objs

# C source file lists
LIBOPUSFILE_CSOURCES = \
http.c \
info.c \
internal.c \
opusfile.c \
stream.c \

LIBOPUSFILE_CHEADERS = \
internal.h \

LIBOPUSURL_CSOURCES = \
internal.c \
http.c \

ifneq ($(findstring mingw,${CC}),)
LIBOPUSURL_CSOURCES += wincerts.c
endif

LIBOPUSURL_CHEADERS = \
internal.h \

OPUSFILE_EXAMPLE_CSOURCES = opusfile_example.c

SEEKING_EXAMPLE_CSOURCES = seeking_example.c

ifneq ($(findstring mingw,${CC}),)
OPUSFILE_EXAMPLE_CSOURCES += win32utf8.c
SEEKING_EXAMPLE_CSOURCES += win32utf8.c
endif

FOO_CSOURCES = tests/foo.c

# Create object file list.
LIBOPUSFILE_OBJS:= ${LIBOPUSFILE_CSOURCES:%.c=${WORKDIR}/%.o}
LIBOPUSFILE_ASMS:= ${LIBOPUSFILE_OBJS:%.o=%.s}
LIBOPUSFILE_DEPS:= ${LIBOPUSFILE_OBJS:%.o=%.d}
LIBOPUSURL_OBJS:= ${LIBOPUSURL_CSOURCES:%.c=${WORKDIR}/%.o}
LIBOPUSURL_ASMS:= ${LIBOPUSURL_OBJS:%.o=%.s}
LIBOPUSURL_DEPS:= ${LIBOPUSURL_OBJS:%.o=%.d}
OPUSFILE_EXAMPLE_OBJS:= ${OPUSFILE_EXAMPLE_CSOURCES:%.c=${WORKDIR}/%.o}
SEEKING_EXAMPLE_OBJS:= ${SEEKING_EXAMPLE_CSOURCES:%.c=${WORKDIR}/%.o}
#TODO: tests
FOO_OBJS:= ${FOO_CSOURCES:%.c=${WORKDIR}/%.o}
ALL_OBJS:= \
 ${LIBOPUSFILE_OBJS} \
 ${LIBOPUSURL_OBJS} \
 ${OPUSFILE_EXAMPLE_OBJS} \
 ${SEEKING_EXAMPLE_OBJS} \

#TODO: tests
# ${FOO_OBJS}

# Create the dependency file list
ALL_DEPS:= ${ALL_OBJS:%.o=%.d}
# Prepend source path to file names.
LIBOPUSFILE_CSOURCES:= ${LIBOPUSFILE_CSOURCES:%=${LIBSRCDIR}/%}
LIBOPUSFILE_CHEADERS:= ${LIBOPUSFILE_CHEADERS:%=${LIBSRCDIR}/%}
LIBOPUSURL_CSOURCES:= ${LIBOPUSURL_CSOURCES:%=${LIBSRCDIR}/%}
LIBOPUSURL_CHEADERS:= ${LIBOPUSURL_CHEADERS:%=${LIBSRCDIR}/%}
OPUSFILE_EXAMPLE_CSOURCES:= ${OPUSFILE_EXAMPLE_CSOURCES:%=${BINSRCDIR}/%}
SEEKING_EXAMPLE_CSOURCES:= ${SEEKING_EXAMPLE_CSOURCES:%=${BINSRCDIR}/%}
#TODO: tests
FOO_CSOURCES:= ${FOO_CSOURCES:%=${TESTSRCDIR}/%}
ALL_CSOURCES:= \
 ${LIBOPUSFILE_CSOURCES} \
 ${LIBOPUSURL_CSOURCES} \
 ${OPUSFILE_EXAMPLE_CSOURCES} \
 ${SEEKING_EXAMPLE_CSOURCES} \

#TODO: tests
# ${FOO_CSOURCES} \
# Prepand target path to file names.
LIBOPUSFILE_TARGET:= ${TARGETLIBDIR}/${LIBOPUSFILE_TARGET}
LIBOPUSURL_TARGET:= ${TARGETLIBDIR}/${LIBOPUSURL_TARGET}
OPUSFILE_EXAMPLE_TARGET:= ${TARGETBINDIR}/${OPUSFILE_EXAMPLE_TARGET}${EXEEXT}
SEEKING_EXAMPLE_TARGET:= ${TARGETBINDIR}/${SEEKING_EXAMPLE_TARGET}${EXEEXT}
# Prepend test path to file names.
#TODO: tests
FOO_TARGET:= ${TESTBINDIR}/${FOO_TARGET}
# Complete set of targets
ALL_TARGETS:= \
 ${LIBOPUSFILE_TARGET} \
 ${LIBOPUSURL_TARGET} \
 ${OPUSFILE_EXAMPLE_TARGET} \
 ${SEEKING_EXAMPLE_TARGET} \

#TODO: tests
# ${FOO_TARGET} \

# Targets:
# Everything (default)
all: ${ALL_TARGETS}

# libopusfile
${LIBOPUSFILE_TARGET}: ${LIBOPUSFILE_OBJS}
	mkdir -p ${TARGETLIBDIR}
	$(AR) cqs $@ ${LIBOPUSFILE_OBJS}
	-$(RANLIB) $@

# libopusurl
${LIBOPUSURL_TARGET}: ${LIBOPUSURL_OBJS}
	mkdir -p ${TARGETLIBDIR}
	$(AR) cqs $@ ${LIBOPUSURL_OBJS}
	-$(RANLIB) $@

# opusfile_example
${OPUSFILE_EXAMPLE_TARGET}: ${OPUSFILE_EXAMPLE_OBJS} ${LIBOPUSFILE_TARGET} \
 ${LIBOPUSURL_TARGET}
	mkdir -p ${TARGETBINDIR}
	${CC} ${CFLAGS} ${OPUSFILE_EXAMPLE_OBJS} ${LIBOPUSFILE_TARGET} \
         ${LIBOPUSURL_TARGET} ${LIBS} -o $@

# seeking_example
${SEEKING_EXAMPLE_TARGET}: ${SEEKING_EXAMPLE_OBJS} ${LIBOPUSFILE_TARGET} \
 ${LIBOPUSURL_TARGET}
	mkdir -p ${TARGETBINDIR}
	${CC} ${CFLAGS} ${SEEKING_EXAMPLE_OBJS} ${LIBOPUSFILE_TARGET} \
         ${LIBOPUSURL_TARGET} ${LIBS} -o $@

#TODO:
#tests: foo
#
#${FOO_TARGET}: ${FOO_OBJS}
#	mkdir -p ${TESTBINDIR}
#	${CC} ${CFLAGS} ${FOO_OBJS} ${FOO_LIBS} -o $@
#
#tests-clean:
#	-rmdir ${TESTBINDIR}

# Assembly listing
ALL_ASM := ${ALL_OBJS:%.o=%.s}
asm: ${ALL_ASM}

# Check that build is complete.
check: all

# Remove all targets.
clean:
	${RM} ${ALL_ASM} ${ALL_OBJS} ${ALL_DEPS}
	${RM} ${ALL_TARGETS}
	-rmdir ${WORKDIR}

# Make everything depend on changes in the Makefile
# This vpath directive needs to be before any include statements
vpath Makefile $(dir $(lastword $(MAKEFILE_LIST)))
${ALL_ASM} ${ALL_OBJS} ${ALL_DEPS} ${ALL_TARGETS} : Makefile

# Specify which targets are phony for GNU make
.PHONY : all clean check

# Rules
${WORKDIR}/%.d: ${LIBSRCDIR}/%.c
	mkdir -p ${dir $@}
	${MAKEDEPEND} ${CINCLUDE} ${CFLAGS} $< -MT ${@:%.d=%.o} > $@
	${MAKEDEPEND} ${CINCLUDE} ${CFLAGS} $< -MT ${@:%.d=%.s} >> $@
	${MAKEDEPEND} ${CINCLUDE} ${CFLAGS} $< -MT $@ >> $@
${WORKDIR}/%.s: ${LIBSRCDIR}/%.c
	mkdir -p ${dir $@}
	${CC} ${CINCLUDE} ${CFLAGS} -S -o $@ $<
${WORKDIR}/%.o: ${LIBSRCDIR}/%.c
	mkdir -p ${dir $@}
	${CC} ${CINCLUDE} ${CFLAGS} -c -o $@ $<

${WORKDIR}/%.d : ${BINSRCDIR}/%.c
	mkdir -p ${dir $@}
	${MAKEDEPEND} ${CINCLUDE} ${CFLAGS} $< -MT ${@:%.d=%.o} > $@
${WORKDIR}/%.s : ${BINSRCDIR}/%.c ${WORKDIR}/%.o
	mkdir -p ${dir $@}
	${CC} ${CINCLUDE} ${CFLAGS} -S -o $@ $<
${WORKDIR}/%.o : ${BINSRCDIR}/%.c
	mkdir -p ${dir $@}
	${CC} ${CINCLUDE} ${CFLAGS} -c -o $@ $<

# Include header file dependencies, except when cleaning
ifneq ($(MAKECMDGOALS),clean)
include ${ALL_DEPS}
endif
