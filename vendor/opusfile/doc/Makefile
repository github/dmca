## GNU makefile for opusfile documentation.

-include ../package_version

all: doxygen

doxygen: Doxyfile ../include/opusfile.h
	doxygen

pdf: doxygen
	make -C latex

clean:
	$(RM) -r html
	$(RM) -r latex

distclean: clean
	$(RM) Doxyfile

.PHONY: all clean distclean doxygen pdf

../package_version:
	@if [ -x ../update_version ]; then \
		../update_version || true; \
	elif [ ! -e $@ ]; then \
		echo 'PACKAGE_VERSION="unknown"' > $@; \
	fi

# run autoconf-like replacements to finalize our config
Doxyfile: Doxyfile.in Makefile ../package_version
	sed -e 's/@PACKAGE_NAME@/opusfile/' \
	    -e 's/@PACKAGE_VERSION@/$(PACKAGE_VERSION)/' \
	    -e 's/@HAVE_DOT@/yes/' \
	    -e 's/@top_srcdir@/../' \
	  < $< > $@
