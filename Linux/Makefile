CFLAGS=-c -Wall -O2 -D__LINUX__ -I../src
LIBS = -lm -lpthread

all: libbb_temperature.a

libbb_temperature.a: bb_temperature.o
	ar -rc libbb_temperature.a bb_temperature.o ;\
	sudo cp libbb_temperature.a /usr/local/lib ;\
	sudo cp ../src/bb_temperature.h /usr/local/include

bb_temperature.o: ../src/linux_io.inl ../src/bb_temperature.cpp ../src/bb_temperature.h
	$(CXX) $(CFLAGS) ../src/bb_temperature.cpp

clean:
	rm *.o libbb_temperature.a
