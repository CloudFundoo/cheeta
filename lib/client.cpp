#include <stdio.h>
#include <unistd.h>

#define SOCK_PATH "/home/nmathew/client"
#define SERVER_ADDR "/home/nmathew/server"
#define CLIENT_SEND_RETRY 5
#include "client.h"

unsigned int client::instance_cnt = 0;

client::client(){
	char name[30];

	instance_cnt++;	
	socketfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
#if 0
	memset(&instanceaddr, 0, sizeof(struct sockaddr_un));
	instanceaddr.sun_family = AF_UNIX;
	instanceaddr.sun_path[0] = 0;
	sprintf(name, "%s%d", SOCK_PATH, instance_cnt);
	strncpy(&(instanceaddr.sun_path[1]), name, 25);
	bind(socketfd, (struct sockaddr *)&instanceaddr, sizeof(struct sockaddr_un));		
#endif
}

client::~client(){
	close(socketfd);
}

int client::connect(){
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	remoteaddr.sin_port = htons(5555);
	return ::connect(socketfd, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr_in));			
}

int client::send(void *buffer, unsigned int size){
	int trancount;
	unsigned int retry = CLIENT_SEND_RETRY;
	while(size > 0 && retry){
	trancount = write(socketfd, buffer, size);
	size -= trancount;
	retry--;
	}
}

int client::recv(void *buffer, unsigned int &size){
	int recvcount;
	size = read(socketfd, buffer, size);
}

