OUTDIR ?= bin
LIBDIR := /usr/lib
SRC    := $(wildcard *.c)
OBJ     = $(SRC:.c=.o)
CFLAGS  = -O2 -g -fno-rtti -fvisibility=hidden  -fvisibility-inlines-hidden

CC      = xcrun clang

.PHONY: all clean

all: $(OUTDIR)/libstringhash.dylib $(OUTDIR)/libstringhash.a $(OUTDIR)/test $(OUTDIR)/libstringarray.dylib $(OUTDIR)/libstringhash.a

%.o: %.c %.d Makefile
	$(CC) -c $(CFLAGS) -o $@ $<

$(OUTDIR)/lib%.dylib: %.o | $(OUTDIR)
	$(CC) -dynamiclib -install_name /usr/lib/lib$(<:.o=.dylib) -o $@ $^

$(OUTDIR)/lib%.a: %.o | $(OUTDIR)
	ar r $@ $^

$(OUTDIR):
	mkdir -p $@

$(OUTDIR)/test: main.c $(OUTDIR)/libstringhash.a $(OUTDIR)/libstringarray.a | $(OUTDIR)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -rf $(OUTDIR) *.o *.d

%.d: %.c
	@echo generating depends for $<
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(SRC:.c=.d)
