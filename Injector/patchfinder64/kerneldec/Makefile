all: kerneldec

bin:
	@mkdir bin

%.o: %.c
	gcc -Wall -MMD -c $< -o $@

%.cpp.o: %.cpp
	g++ -Wall -MMD -c $< -o $@

-include $(wildcard *.d)

kerneldec: kerneldec.cpp.o lzssdec.cpp.o main.o
	g++ -Wall $^ -o $@

clean:
	rm -f kerneldec *.o *.d
