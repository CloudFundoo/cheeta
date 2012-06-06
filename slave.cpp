#include <stdio.h>
#include "client.h"

int main(void)
{
	class client *pclient = new client();
	char sendbuffer[100] = "I am fucked and you?";
	char recvbuffer[100];
	unsigned int recvsize = 100;

	if(!pclient->connect())
	{
		pclient->send((void *)sendbuffer, strlen(sendbuffer));
		pclient->recv((void *)recvbuffer, recvsize);
	}
	else {
		printf("client::error in connect\n");
	}	

	return 0;
}
