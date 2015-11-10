all: build clean

build: main.o global.o udp-send.o udp-recv.o
	gcc main.o global.o udp-send.o udp-recv.o -o head_or_tail -g

main.o: main.c udp-send.h udp-recv.h global.h
	gcc -c main.c	

global.o: global.c global.h
	gcc -c global.c

udp-send.o: udp-send.c udp-send.h global.h
	gcc -c udp-send.c

udp-recv.o: udp-recv.c udp-recv.h global.h
	gcc -c udp-recv.c


clean: 
	rm *.o
