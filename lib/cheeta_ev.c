#include <stdio.h>
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
	cheeta->polledfdscount = 0;
	
	return cheeta;
}

int cheeta_event_get(struct cheeta_context *context, struct eventfd *(**eventbuffer)[1], unsigned int buffersize) 
{
	int eventfdcount;
	struct epoll_event *onevent;
	struct eventfd *(*tempeventbuffer)[1];
	
	if(!context->polledfdscount)
		return -1;
	
	onevent = (struct epoll_event *)malloc(context->polledfdscount * sizeof(struct epoll_event));
	
	eventfdcount = epoll_wait(context->epfd, onevent, context->polledfdscount, 0);
	tempeventbuffer = (struct eventfd *(*)[1])malloc(eventfdcount * sizeof(struct eventfd *));
	*eventbuffer = tempeventbuffer;
	
	if(eventfdcount > 0)
	{	
		int i = 0;
		for(;i < eventfdcount; i++)
		{
			(*tempeventbuffer)[i] = (struct eventfd *)(onevent[i].data.ptr);
			((*tempeventbuffer)[i])->out_event = onevent[i].events;
		}		
	}
	free(onevent);
	return eventfdcount;
}

unsigned int cheeta_add_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{	
	struct epoll_event event2add;

	event2add.data.ptr = event;
	event2add.events = event->in_event;
	context->polledfdscount++;

	return epoll_ctl(context->epfd, EPOLL_CTL_ADD, event->fd, &event2add);			
}
unsigned int cheeta_remove_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{
	struct epoll_event event2remove;
	
	event2remove.data.ptr = event;
	event2remove.events = 0;
	context->polledfdscount--;
	
	return epoll_ctl(context->epfd, EPOLL_CTL_DEL, event->fd, &event2remove);
}

inline unsigned int cheeta_modify_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{
	struct epoll_event event2remove;
	
	event2remove.data.ptr = event;
	event2remove.events = (event->in_event|EPOLLET);
	
	return epoll_ctl(context->epfd, EPOLL_CTL_MOD, event->fd, &event2remove);
}
