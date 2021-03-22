CC = g++
CFLAGS  = --std=c++11 -O3 -Wall -Wextra -Wno-unknown-pragmas -I../../Lib

TARGET = Example
SRCS = main.cpp DetectDebugger.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	-rm *.o $(TARGET)
