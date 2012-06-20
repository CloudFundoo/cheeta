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
				if(eventbuffer[i].data.fd == pserser->socketfd)
				{
					int newfd;

					if((newfd =	accept(pserver->socketfd, NULL, NULL)) > 0)
					{
						struct eventfd newevent2add;

						newevent2add.events = CH_EV_READ|CH_EV_WRITE;
						newevent2add.data.fd = newfd;
						cheeta_add_eventfd(handle, &newevent2add, 0);
						handle->sessions[newfd-4] = newfd;
					}
				}
				else {
					handle->sessions[eventbuffer[i].data.fd-4].responsesize = 
					read(eventbuffer[i].data.fd, handle->sessions[eventbuffer[i].data.fd-4], 4092);
					vizsla_processs_http(handle->sessions[eventbuffer[i].data.fd-4]);	
				}
			}	
			else if(eventbuffer[i].out_event &CH_EV_WRITE)
			{	
				if(handle->sessions[eventbuffer[i].data.fd-4].ready4write)
				{
					int writecount = 0;
					
					writecount -= write(eventbuffer[i].data.fd, handle->sessions[eventbuffer[i].data.fd-4], handle->sessions[eventbuffer[i].data.fd-4].writesize;
					handle->sessions[eventbuffer[i].data.fd-4].writesize -= writecount;	
				}
			}
			i--;			
		}
	}
	return 0;
}