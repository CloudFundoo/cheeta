#include "server.h"

int main(void)
{
	class server *pserver = new server();
	if(pserver->waitforclient())
	{
		pserver->recv();
		pserver->send();
	}

	return 0;
}
