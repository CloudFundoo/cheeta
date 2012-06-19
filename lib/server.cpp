
#include <stdio.h>
#include <unistd.h>

#define SERVER_ADDR "/home/nmathew/server"
#define SERVER_SEND_RETRY 5
#include "server.h"


server::server(){
        char name[30];
        socketfd = socket(AF_UNIX, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if(socketfd<0)
		printf("Error on socket create\n");
		
        memset(&instanceaddr, 0, sizeof(struct sockaddr_un));
        instanceaddr.sun_family = AF_UNIX;
        instanceaddr.sun_path[0] = 0;
        strncpy(&(instanceaddr.sun_path[1]), SERVER_ADDR, 25);
      	bind(socketfd, (struct sockaddr *)&instanceaddr, sizeof(struct sockaddr_un));
	listen(socketfd, SOMAXCONN);
}

server::~server(){
	close(socketfd);
}

