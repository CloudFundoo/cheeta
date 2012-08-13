#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "server.h"
#include "cheeta_ev.h"
#include "vizsla.h"

struct connection
{
    int state;
    char buffer[4096];
	int requestsize;
	int responsesize;
	int ready4write;
	int writesize;
};


void vizsla_process_http(struct cheeta_session *);
void vizsla_process_http(struct connection *connection)
{
	memset(connection->buffer, 0, 13);
	strncpy(connection->buffer, "Hello World!", 12);
	connection->buffer[12] = '\0';
	connection->writesize = 13;
	connection->ready4write = 1;
	return;
}

void *vizsla_cpu_eventloop_threadfunc(void *arg)
{
	struct tcpu_info *ptcpuinfo;
	struct eventfd *(*eventbuffer)[1];
	struct eventfd *addevent;
	struct eventfd *removeevent;
	struct eventfd *currevent;
	struct connection *curconnection = NULL;
	struct cheeta_context *cheeta_thandle = cheeta_event_init();
	
	ptcpuinfo = (struct tcpu_info *)arg;
	addevent = (struct eventfd *)malloc(sizeof(struct eventfd));

	addevent->fd = ptcpuinfo->listenerfd;
	addevent->in_event = CH_EV_READ|EPOLLET;
	cheeta_add_eventfd(cheeta_thandle, addevent, sizeof(addevent)/sizeof(struct eventfd));

	for(;;)
	{	
		int eventcount, k, i;
		
		eventcount = cheeta_event_get(cheeta_thandle, &eventbuffer, 32);
		k = eventcount;
		while(k > 0)
		{
			i = k-1;	
			k--;			
			currevent = (*eventbuffer)[i];
			curconnection = (struct connection *)currevent->ptr;
			if(((currevent->out_event & EPOLLHUP) || (currevent->out_event & EPOLLERR)) && 
						(currevent->fd != ptcpuinfo->listenerfd))
			{
				removeevent = currevent;
				cheeta_remove_eventfd(cheeta_thandle, removeevent, 0);
				close(removeevent->fd);
				if(removeevent->ptr);
                    free(removeevent->ptr);
                if(removeevent);
                	free(removeevent);
				continue;
			}
			if(currevent->out_event & CH_EV_READ)  
			{	
				if(currevent->fd == ptcpuinfo->listenerfd)
				{
					int newfd;
					struct connection *pnewconnection = NULL;

					while((newfd =	accept4(ptcpuinfo->listenerfd, NULL, NULL, SOCK_NONBLOCK)) > 0)
					{
						addevent = (struct eventfd *)malloc(sizeof(struct eventfd));
						addevent->in_event = CH_EV_READ|CH_EV_WRITE;
						pnewconnection = (struct connection *)malloc(sizeof(struct connection));
						addevent->fd = newfd;
						addevent->ptr = pnewconnection;
						cheeta_add_eventfd(cheeta_thandle, addevent, 0);
					}
				}
				else {
					curconnection->responsesize = read(currevent->fd, curconnection->buffer, 4092);
					vizsla_process_http(curconnection);	
				}
				continue;
			}	
			if(currevent->out_event & CH_EV_WRITE)
			{	
				if((curconnection->ready4write) && (curconnection->writesize))
				{
					int writecount = 0;

					writecount = write(currevent->fd, curconnection->buffer, curconnection->writesize);
					curconnection->writesize -= writecount;	
					if(!(curconnection->writesize))
						curconnection->ready4write = 0;
				}
				continue;
			}
		}
		free(eventbuffer);
	}
}

int main(void)
{
	class server *pserver = new server();
	unsigned int recvlength = 25;
	unsigned int configuredcpuscount = 0;
	unsigned int onlinecpuscount = 0;
	pthread_t threadid;
	pthread_attr_t thread_attr;
	int ret;
	int threadcount = 0;
	struct tcpu_info *ptcpuinfo;
	struct sched_param tsched_param;
	cpu_set_t cpuset;

	configuredcpuscount = sysconf(_SC_NPROCESSORS_CONF);
	onlinecpuscount = sysconf(_SC_NPROCESSORS_ONLN);
	
	tsched_param.sched_priority = sched_get_priority_max(SCHED_RR);	

	threadcount = onlinecpuscount;
	while(threadcount){
		pthread_attr_init(&thread_attr);
		pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
		pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
		pthread_attr_setschedparam(&thread_attr, &tsched_param);	
		CPU_ZERO(&cpuset);
        CPU_SET(threadcount-1, &cpuset);
		pthread_attr_setaffinity_np(&thread_attr, sizeof(cpuset), &cpuset);
		ptcpuinfo = (struct tcpu_info *)malloc(sizeof(struct tcpu_info));
		memset(ptcpuinfo, 0, sizeof(struct tcpu_info));
		ptcpuinfo->listenerfd = pserver->socketfd;

		if((ret = pthread_create(&threadid, &thread_attr, vizsla_cpu_eventloop_threadfunc, ptcpuinfo) != 0))
		{
			perror("pthread_create failed");
			return ret;
		}
		threadcount--;
	}

	while(1);

	return 0;
}
