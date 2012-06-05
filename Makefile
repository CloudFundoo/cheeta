CFLAGS=-I./lib/
LDFLAGS=-L./lib

all:
	make -C lib/ all
	g++ -o slave slave.cpp $(CFLAGS) $(LDFLAGS) -lcheeta
	g++ -o master master.cpp $(CFLAGS) $(LDFLAGS) -lcheeta

clean:
	make -C lib/ clean
	rm slave
	rm master
