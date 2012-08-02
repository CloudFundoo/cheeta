#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "server.h"
#include "cheeta_ev.h"
#include "vizsla.h"

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

void *vizsla_cpu_eventloop_threadfunc(void *arg)
{
	struct tcpu_info *ptcpuinfo;
	struct sched_param tsched_param;
	cpu_set_t cpuset;
	struct eventfd eventbuffer[32];
	struct eventfd addevent;
	struct cheeta_context *cheeta_thandle = cheeta_event_init();
	
	ptcpuinfo = (struct tcpu_info *)arg;
	tsched_param.sched_priority = sched_get_priority_max(SCHED_RR);
	pthread_setschedparam(pthread_self(), SCHED_RR, &tsched_param);

	CPU_ZERO(&cpuset);
	CPU_SET(ptcpuinfo->cpu_num, &cpuset);
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset); 
	pthread_yield();

	addevent.fd = ptcpuinfo->listenerfd;
	addevent.in_event = CH_EV_READ|EPOLLET;
	cheeta_add_eventfd(cheeta_thandle, &addevent, sizeof(addevent)/sizeof(struct eventfd));

	for(;;)
	{	
		int eventcount, k, i;
		
		eventcount = cheeta_event_get(cheeta_thandle, &eventbuffer[0], 32);
		k = eventcount;
		while(k > 0)
		{
			i = k-1;	
			if(((eventbuffer[i].out_event & EPOLLHUP) || (eventbuffer[i].out_event & EPOLLERR)) && (eventbuffer[i].fd != ptcpuinfo->listenerfd))
			{
				struct eventfd event2remove;

				event2remove.fd = eventbuffer[i].fd;
				cheeta_remove_eventfd(cheeta_thandle, &event2remove, 0);
				close(eventbuffer[i].fd);
			
			}
			if(eventbuffer[i].out_event & CH_EV_READ)  
			{	
				if(eventbuffer[i].fd == ptcpuinfo->listenerfd)
				{
					int newfd;

					while((newfd =	accept(ptcpuinfo->listenerfd, NULL, NULL)) > 0)
					{
						struct eventfd newevent2add;

						newevent2add.in_event = CH_EV_READ|CH_EV_WRITE;
						newevent2add.fd = newfd;
						cheeta_add_eventfd(cheeta_thandle, &newevent2add, 0);
						cheeta_thandle->sessions[newfd-4].remotefd = newfd;
					}
				}
				else {
					cheeta_thandle->sessions[eventbuffer[i].fd-4].responsesize = 
					read(eventbuffer[i].fd, cheeta_thandle->sessions[eventbuffer[i].fd-4].buffer, 4092);
					vizsla_process_http(&(cheeta_thandle->sessions[eventbuffer[i].fd-4]));	
				}
			}	
			if(eventbuffer[i].out_event & CH_EV_WRITE)
			{	
				if((cheeta_thandle->sessions[eventbuffer[i].fd-4].ready4write) && (cheeta_thandle->sessions[eventbuffer[i].fd-4].writesize))
				{
					int writecount = 0;
					writecount = write(eventbuffer[i].fd, &(cheeta_thandle->sessions[eventbuffer[i].fd-4].buffer), cheeta_thandle->sessions[eventbuffer[i].fd-4].writesize);
					printf("wrote %d bytes\n", writecount);
					cheeta_thandle->sessions[eventbuffer[i].fd-4].writesize -= writecount;	
					if(!(cheeta_thandle->sessions[eventbuffer[i].fd-4].writesize))
						cheeta_thandle->sessions[eventbuffer[i].fd-4].ready4write = 0;
				}
			}
			k--;			
		}
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

	configuredcpuscount = sysconf(_SC_NPROCESSORS_CONF);
	onlinecpuscount = sysconf(_SC_NPROCESSORS_ONLN);
	
	threadcount = onlinecpuscount;
	while(threadcount){
		pthread_attr_init(&thread_attr);
		pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
		ptcpuinfo = (struct tcpu_info *)malloc(sizeof(struct tcpu_info));
		memset(ptcpuinfo, 0, sizeof(struct tcpu_info));
		ptcpuinfo->cpu_num = threadcount;
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
