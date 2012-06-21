#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>

#include "cheeta_ev.h"

struct cheeta_context *cheeta_event_init() 
{
	struct cheeta_context *cheeta = (struct cheeta_context *)
				malloc(sizeof(struct cheeta_context));
	cheeta->epfd = epoll_create(1024);	
	
	return cheeta;
}

unsigned int cheeta_event_get(struct cheeta_context *context, struct eventfd *eventbuffer, unsigned int buffersize) 
{
	int eventfdcount;
	eventfdcount = epoll_wait(context->epfd, context->onevent, 32, 0);
	if(eventfdcount > 0)
	{	
		int i = 0;
		for(;i < eventfdcount; i++)
		{
			eventbuffer[i].fd = context->onevent[i].data.fd;
			eventbuffer[i].out_event = context->onevent[i].events;
		}		
	}
	return eventfdcount;
}

unsigned int cheeta_add_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{	
	struct epoll_event event2add;

	event2add.data.fd = event->fd;
	event2add.events = event->in_event;

	return epoll_ctl(context->epfd, EPOLL_CTL_ADD, event2add.data.fd, &event2add);			
}
inline unsigned int cheeta_remove_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{
	struct epoll_event event2remove;
	
	event2remove.data.fd = event->fd;
	event2remove.events = 0;
	
	return epoll_ctl(context->epfd, EPOLL_CTL_DEL, event2remove.data.fd, &event2remove);
}

inline unsigned int cheeta_modify_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{
	struct epoll_event event2remove;
	
	event2remove.data.fd = event->fd;
	event2remove.events = event->in_event;
	
	return epoll_ctl(context->epfd, EPOLL_CTL_MOD, event2remove.data.fd, &event2remove);
}
