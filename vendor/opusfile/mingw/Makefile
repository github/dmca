# Cross-compile opusfile under mingw

TOOL_PREFIX ?= i686-w64-mingw32

# To build opusfile under mingw, we first need to build:
DEPS = ogg opus ssl

ogg_URL := https://downloads.xiph.org/releases/ogg/libogg-1.3.4.tar.xz
ogg_SHA := c163bc12bc300c401b6aa35907ac682671ea376f13ae0969a220f7ddf71893fe

opus_URL := https://archive.mozilla.org/pub/opus/opus-1.3.1.tar.gz
opus_SHA := 65b58e1e25b2a114157014736a3d9dfeaad8d41be1c8179866f144a2fb44ff9d

ssl_URL := https://openssl.org/source/openssl-1.0.2u.tar.gz
ssl_SHA := ecd0c6ffb493dd06707d38b14bb4d8c2288bb7033735606569d8f90f89669d16

all: $(DEPS)

libopusfile-0.dll: ../unix/Makefile $(DEPS)
	CC=$(TOOL_PREFIX)-gcc \
	RANLIB=$(TOOL_PREFIX)-ranlib \
	PKG_CONFIG_PATH=$(CURDIR)/lib/pkgconfig \
	$(MAKE) -f $<

opusfile: $(DEPS)
	$(MKDIR) $@
	cd $@ && ../configure --host=$(TOOL_PREFIX) --prefix=$(CURDIR) \
	  PKG_CONFIG_PATH=$(CURDIR)/lib/pkgconfig
	$(MAKE) -C $@

clean:
	$(RM) -r objs
	$(RM) -r  bin include lib share ssl
	$(RM) -r $(DEP_DIRS)
	$(RM) opusfile_example.exe seeking_example.exe
	$(RM) libopusfile.a libopusurl.a

# Generate rules to download and verify each dependency.
define WGET_template =
 # Generate tarball name from the url.
 DEP_TARBALLS += $$(notdir $$($(1)_URL))
 $(1)_DIR := $$(basename $$(basename $$(notdir $$($(1)_URL))))
 DEP_DIRS += $$($(1)_DIR)

 # Verify and unpack tarball.
 $$($(1)_DIR): $$(notdir $$($(1)_URL))
	@if test "$$($(1)_SHA)" = "$$$$(sha256sum $$< | cut -f 1 -d ' ')"; \
	then \
	  echo "+ $$< checksum verified."; \
	else \
	  echo "! $$< checksum didn't match!"; \
	  $(RM) $$<; exit 1; \
	fi
	tar xf $$<

 # Fetch tarball from the url.
 $$(notdir $$($(1)_URL)):
	wget $$($(1)_URL)

 # Hook project-specific build rule.
 $(1): $(1)_BUILD
endef
$(foreach dep,$(DEPS),$(eval $(call WGET_template,$(dep))))

fetch: $(DEP_TARBALLS)

realclean: clean
	$(RM) $(DEP_TARBALLS)

# Build scripts for each specific target.

# NOTE: 'make check' generally requires wine with cross-compiling.
ogg_BUILD: $(ogg_DIR)
	cd $< && ./configure --host=$(TOOL_PREFIX) --prefix=$(CURDIR)
	$(MAKE) -C $< install

opus_BUILD: $(opus_DIR)
	cd $< && ./configure --host=$(TOOL_PREFIX) --prefix=$(CURDIR)
	$(MAKE) -C $< install

ssl_BUILD: $(ssl_DIR)
	cd $< && ./Configure mingw \
	  --prefix=$(CURDIR) \
	  --cross-compile-prefix=$(TOOL_PREFIX)-
	$(MAKE) -C $< depend
	$(MAKE) -C $<
	$(MAKE) -C $< install

# Package the binaries.
DIST_VERSION := $(shell git describe --dirty)
DIST := opusfile-$(DIST_VERSION)-win32
package: $(DIST).zip

$(DIST).zip: $(DIST)
	zip -r $@ $</*
	@echo $@ ready to go.

$(DIST): $(addprefix $(CURDIR)/bin/, libogg-0.dll libopus-0.dll)
	#cd .. && make install
	mkdir -p $(DIST)
	cp ../AUTHORS ../COPYING ../README.md ../include/opusfile.h $@
	cp .libs/libopusfile-0.dll $@
	cp .libs/libopusfile.a $@
	cp .libs/libopusurl-0.dll $@
	cp .libs/libopusurl.a $@
	cp bin/*.dll $@
	cp *.exe $@
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libgcc_s_dw2-1.dll $@
	cp /usr/i686-w64-mingw32/sys-root/mingw/bin/libwinpthread-1.dll $@
	i686-w64-mingw32-strip $@/*.exe
	i686-w64-mingw32-strip $@/*.dll
	i686-w64-mingw32-strip $@/*.a
	cd $@ && sha256sum * > SHA256SUMS.txt
