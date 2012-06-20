#include <stdio.h>
#include "server.h"
#include "cheeta_ev.h"

int main(void)
{
	class server *pserver = new server();
	char recvbuffer[100];
	unsigned int recvlength = 25;
	struct eventfd addevent;
	struct cheeta_context *handle = cheeta_event_init();
	struct eventfd eventbuffer[32];

	addevent.fd = pserver->socketfd;
	addevent.in_event = CH_EV_READ;
	cheeta_add_eventfd(handle, &addevent, sizeof(addevent)/sizeof(struct eventfd));

	for(;;)
	{	
		int eventcount, i, accepted;
		
		eventcount = cheeta_get_event(handle, &eventbuffer, 32);
		i = eventcount;
		while(i > 0)
		{	
			if(eventbuffer[i].out_event & CH_EV_READ)  
			{	
				if(!accepted)
					if(eventbuffer[i].data.fd == perser->socketfd)
 
			}	
			else if(eventbuffer[i].out_event &CH_EV_WRITE)
			{
				
			}
			i--;			
		}
	}

	return 0;
}
