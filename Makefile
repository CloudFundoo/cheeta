CFLAGS=-I./lib/
LDFLAGS=-L./lib

all:
	make -C lib/ all
	g++ -g -o slave slave.cpp $(CFLAGS) $(LDFLAGS) -lcheeta -lpthread
	g++ -o vizsla vizsla.cpp $(CFLAGS) $(LDFLAGS) -lcheeta -lpthread

clean:
	make -C lib/ clean
	rm slave
	rm vizsla
