all:
	g++ -c -fPIC server.cpp -o server.o
	g++ -c -fPIC client.cpp -o client.o
	g++ -c -fPIC cheeta_ev.c -o cheeta_ev.o
	g++ -shared -o libcheeta.so server.o client.o cheeta_ev.o

clean:
	rm *.o
	rm libcheeta.so
	
