#include <stdio.h>
#include "server.h"

int main(void)
{
	class server *pserver = new server();
	char recvbuffer[100];
	unsigned int recvlength = 25;

	if(!(pserver->waitforclient()))
	{
		pserver->recv((void *)recvbuffer, recvlength);
		recvbuffer[recvlength + 1] = 31;
		pserver->send((void *)recvbuffer, (recvlength + 1));
	}
	else{
		printf("No connections\n");
	}
	

	return 0;
}
