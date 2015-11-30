CFLAG=  -lpcap -lnet -O2 -std=c++11
OBJS = lpcap.o llibnet.o log.o


gossip: gossipCompleto
	rm -f *.o

gossipCompleto: $(OBJS)  gossip.o
	g++  $(OBJS) $(CFLAG)  gossip.o -o gossip

lpcap.o: src/lpcap.cpp
	g++ -c $(CFLAG) src/lpcap.cpp

llibnet.o: src/llibnet.cpp
	g++ -c $(CFLAG) src/llibnet.cpp

log.o: src/log.cpp
	g++ -c $(CFLAG) src/log.cpp

#######################
# Principal
#######################
gossip.o: gossip.cpp
	g++ -c $(CFLAG) gossip.cpp