CFLAGS=-I./lib/
LDFLAGS=-L./lib

all:
	make -C lib/ all
	g++ -o slave slave.cpp $(CFLAGS) $(LDFLAGS) -lcheeta
	g++ -o vizsla vizsla.cpp $(CFLAGS) $(LDFLAGS) -lcheeta

clean:
	make -C lib/ clean
	rm slave
	rm vizsla
