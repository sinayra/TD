CFLAG=  -lpcap -lnet -O2
PATHSRC ='.\src\'
OBJS = lpcap.o


gossip: gossipCompleto
	rm -f *.o

gossipCompleto: $(OBJS)  gossip.o
	g++  $(OBJS) $(CFLAG)  gossip.o -o gossip

lpcap.o: src/lpcap.cpp
	g++ -c $(CFLAG) src/lpcap.cpp

#######################
# Principal
#######################
gossip.o: gossip.cpp
	g++ -c $(CFLAG) gossip.cpp