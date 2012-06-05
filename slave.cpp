#include "client.h"

int main(void)
{
	class client *pclient = new client();
	if(pclient->connect())
	{
		pclient->send();
		pclient->recv();
	}

	return 0;
}
