
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SERVER_ADDR "/home/nmathew/server"
#define SERVER_SEND_RETRY 5
#include "server.h"


server::server(){
	char name[30];

	socketfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if(socketfd<0)
		printf("Error on socket create\n");
		
	memset(&instanceaddr, 0, sizeof(struct sockaddr_in));
	instanceaddr.sin_family = AF_INET;
	instanceaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	instanceaddr.sin_port = htons(5555);
	bind(socketfd, (struct sockaddr *)&instanceaddr, sizeof(struct sockaddr_in));
	listen(socketfd, SOMAXCONN);
}

server::~server(){
	close(socketfd);
}

