all:
	g++ -c -fPIC server.cpp -o server.o
	g++ -c -fPIC client.cpp -o client.o
	g++ -shared -o libcheeta.so server.o client.o

clean:
	rm *.o
	rm libcheeta.so
	
