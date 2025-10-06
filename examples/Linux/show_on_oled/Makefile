CFLAGS= -D__LINUX__ -c -Wall -O2
LIBS = -lm -lOneBitDisplay -lbb_temperature -lpthread -lgpiod

all: show_on_oled

show_on_oled: main.o
	g++ main.o $(LIBS) -o show_on_oled

main.o: main.cpp
	g++ $(CFLAGS) main.cpp

clean:
	rm *.o show_on_oled
