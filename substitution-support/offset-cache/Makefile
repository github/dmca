TARGET  = cachetest
OUTDIR ?= bin
SRC     = $(wildcard *.c)
OBJ	= $(SRC:.c=.o)
CFLAGS  = -DMAIN

CC      = xcrun gcc

.PHONY: all clean

all: $(OUTDIR)/$(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^

$(OUTDIR):
	mkdir -p $@

$(OUTDIR)/$(TARGET): $(OBJ) | $(OUTDIR)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -rf $(OUTDIR) $(OBJ)
