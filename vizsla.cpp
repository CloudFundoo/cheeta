#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "cheeta_ev.h"

void vizsla_process_http(struct cheeta_session *);
void vizsla_process_http(struct cheeta_session *httpsession)
{
	memset(httpsession->buffer, 0, 13);
	strncpy(httpsession->buffer, "Hello World!", 12);
	httpsession->buffer[12] = '\0';
	httpsession->writesize = 13;
	httpsession->ready4write = 1;
	return;
}

int main(void)
{
	class server *pserver = new server();
	unsigned int recvlength = 25;
	struct eventfd addevent;
	struct cheeta_context *handle = cheeta_event_init();
	struct eventfd eventbuffer[32];

	addevent.fd = pserver->socketfd;
	addevent.in_event = CH_EV_READ;
	cheeta_add_eventfd(handle, &addevent, sizeof(addevent)/sizeof(struct eventfd));

	for(;;)
	{	
		int eventcount, k, i;
		
		eventcount = cheeta_event_get(handle, &eventbuffer[0], 32);
		k = eventcount;
		while(k > 0)
		{
			i = k-1;	
			if(((eventbuffer[i].out_event & EPOLLHUP) || (eventbuffer[i].out_event & EPOLLERR)) && (eventbuffer[i].fd != pserver->socketfd))
			{
				struct eventfd event2remove;

				event2remove.fd = eventbuffer[i].fd;
				cheeta_remove_eventfd(handle, &event2remove, 0);
				close(eventbuffer[i].fd);
			
			}
			if(eventbuffer[i].out_event & CH_EV_READ)  
			{	
				if(eventbuffer[i].fd == pserver->socketfd)
				{
					int newfd;

					if((newfd =	accept(pserver->socketfd, NULL, NULL)) > 0)
					{
						struct eventfd newevent2add;

						newevent2add.in_event = CH_EV_READ|CH_EV_WRITE;
						newevent2add.fd = newfd;
						cheeta_add_eventfd(handle, &newevent2add, 0);
						handle->sessions[newfd-4].remotefd = newfd;
					}
				}
				else {
					handle->sessions[eventbuffer[i].fd-4].responsesize = 
					read(eventbuffer[i].fd, handle->sessions[eventbuffer[i].fd-4].buffer, 4092);
					vizsla_process_http(&(handle->sessions[eventbuffer[i].fd-4]));	
				}
			}	
			if(eventbuffer[i].out_event & CH_EV_WRITE)
			{	
				if((handle->sessions[eventbuffer[i].fd-4].ready4write) && (handle->sessions[eventbuffer[i].fd-4].writesize))
				{
					int writecount = 0;
					writecount = write(eventbuffer[i].fd, &(handle->sessions[eventbuffer[i].fd-4].buffer), handle->sessions[eventbuffer[i].fd-4].writesize);
					printf("wrote %d bytes\n", writecount);
					handle->sessions[eventbuffer[i].fd-4].writesize -= writecount;	
					if(!(handle->sessions[eventbuffer[i].fd-4].writesize))
						handle->sessions[eventbuffer[i].fd-4].ready4write = 0;
				}
			}
			k--;			
		}
	}
	return 0;
}
