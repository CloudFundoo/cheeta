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

unsigned int cheeta_event_get(struct cheeta_context *context, struct eventfd *(*eventbuffer)[1], unsigned int buffersize) 
{
	int eventfdcount;
	eventfdcount = epoll_wait(context->epfd, context->onevent, 32, 0);
	if(eventfdcount > 0)
	{	
		int i = 0;
		for(;i < eventfdcount; i++)
		{
			(*eventbuffer)[i] = (struct eventfd *)(context->onevent[i].data.ptr);
			((*eventbuffer)[i])->out_event = context->onevent[i].events;
		}		
	}
	return eventfdcount;
}

unsigned int cheeta_add_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{	
	struct epoll_event event2add;

	event2add.data.ptr = event;
	event2add.events = event->in_event;

	return epoll_ctl(context->epfd, EPOLL_CTL_ADD, event->fd, &event2add);			
}
unsigned int cheeta_remove_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{
	struct epoll_event event2remove;
	
	event2remove.data.ptr = event;
	event2remove.events = 0;
	
	return epoll_ctl(context->epfd, EPOLL_CTL_DEL, event->fd, &event2remove);
}

inline unsigned int cheeta_modify_eventfd(struct cheeta_context *context, struct eventfd *event, unsigned int size)
{
	struct epoll_event event2remove;
	
	event2remove.data.ptr = event;
	event2remove.events = (event->in_event|EPOLLET);
	
	return epoll_ctl(context->epfd, EPOLL_CTL_MOD, event->fd, &event2remove);
}
