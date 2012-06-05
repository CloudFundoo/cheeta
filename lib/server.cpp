
#include <stdio.h>
#include <unistd.h>

#define SERVER_ADDR "/home/nmathew/server"
#define SERVER_SEND_RETRY 5
#include "server.h"

server::server(){
        char name[30];
        socketfd = socket(AF_UNIX, SOCK_STREAM, 0);
        memset(&instanceaddr, 0, sizeof(struct sockaddr_un));
        instanceaddr.sun_family = AF_UNIX;
        instanceaddr.sun_path[0] = '\0';
        strncpy(&(instanceaddr.sun_path[1]), SERVER_ADDR, 25);
      	bind(socketfd, (struct sockaddr *)&instanceaddr, sizeof(struct sockaddr_un));
	listen(socketfd, 16);
}

server::~server(){
	close(socketfd);
}

int server::connect(void){
        remoteaddr.sun_family = AF_UNIX;
        strncpy(remoteaddr.sun_path, SERVER_ADDR, 25);
        ::connect(socketfd, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr_un));
}

int server::send(void *buffer, unsigned int size){
        int trancount;
        unsigned int retry = SERVER_SEND_RETRY;
        while(size > 0 && retry){
        trancount = write(socketfd, buffer, size);
        size -= trancount;
        retry--;
        }
}

int server::recv(void *buffer, unsigned int &size){
        int recvcount;
        size = read(socketfd, buffer, size);
}

int server::waitforclient(){
	int rsocketfd;
	int rsocketaddrsize;
	rsocketfd = accept(socketfd, (struct sockaddr *)&remoteaddr, (socklen_t *)&rsocketaddrsize);
}
