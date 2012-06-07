
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

int server::send(void *buffer, unsigned int size){
        int trancount;
        unsigned int retry = SERVER_SEND_RETRY;
        while(size > 0 && retry){
        trancount = write(rsocketfd, buffer, size);
        size -= trancount;
        retry--;
        }
}

int server::recv(void *buffer, unsigned int &size){
        int recvcount;
        size = read(rsocketfd, buffer, size);
}

int server::waitforclient(){
	int rsocketaddrsize;
	int fdmax;
	fd_set readfdset;
	fd_set masterset;
	FD_ZERO(&readfdset);
	FD_ZERO(&masterset);
	FD_SET(socketfd, &masterset);
	fdmax = socketfd;
	readfdset = masterset;
	if(select(fdmax+1, &readfdset, NULL, NULL, NULL))
	{
		int i = 0;
		for(; i <= fdmax; i++)
			if(FD_ISSET(i, &readfdset))
				if(i = socketfd)
				{
					rsocketfd = accept(socketfd, (struct sockaddr *)&remoteaddr, (socklen_t *)&rsocketaddrsize);
					break;
				}
	}
				
	return 0;
}
