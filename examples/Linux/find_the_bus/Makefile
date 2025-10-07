CFLAGS= -D__LINUX__ -c -Wall -O2
LIBS = -lm -lbb_temperature -lpthread

all: find_the_bus

find_the_bus: main.o
	g++ main.o $(LIBS) -o find_the_bus

main.o: main.cpp
	g++ $(CFLAGS) main.cpp

clean:
	rm *.o find_the_bus
