#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "slave.h"
#include "cheeta_ev.h"

#define ON_CONNECT	0
#define CONNECTED	1

struct connection
{
	int state;
	char buffer[4096];
};


void *vizsla_client_event_loop(void * arg)
{
	int ret;
	struct tcpu_info *ptcpuinfo;
	unsigned int treq_per_thread;
	unsigned int tconcurr_per_thread;
	int *socketfds;
	int *currentsocketfd;
	unsigned int loop = 0;
	struct cheeta_context *cheeta_thandle = cheeta_event_init();
	struct sockaddr_in serveraddr;
	struct eventfd *addevent;
	struct eventfd *removeevent;
	struct eventfd *currevent;
	struct eventfd *(*eventbuffer)[1];
	struct connection *connections;
	char sendbuffer[100] = "I am fucked and you?";
	char recvbuffer[100];
	unsigned int recvsize = 100;

	ptcpuinfo = (struct tcpu_info *)arg;
	tconcurr_per_thread = ptcpuinfo->tconcurr_req_thread;
	treq_per_thread = ptcpuinfo->treq_thread; 

	socketfds = (int *)malloc(tconcurr_per_thread * sizeof(int));
	loop = tconcurr_per_thread;
	
	currentsocketfd = socketfds;
	while(loop--)
	{
		*currentsocketfd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
		
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = inet_addr(ptcpuinfo->hostaddr);
		serveraddr.sin_port = htons(atoi(ptcpuinfo->hostport));
		if((ret = connect(*currentsocketfd, (struct sockaddr *)&serveraddr, sizeof(sockaddr_in))) != 0)
		{
			if(errno == EINPROGRESS)
			{	
				struct connection *pconnection = NULL;

				addevent = (struct eventfd *)malloc(sizeof(struct eventfd));
				addevent->fd = *currentsocketfd;				
				pconnection = (struct connection *)malloc(sizeof(struct connection));	
				pconnection->state = ON_CONNECT;
    			addevent->in_event = CH_EV_WRITE|CH_EV_READ;
				addevent->ptr = pconnection;
    			cheeta_add_eventfd(cheeta_thandle, addevent, sizeof(addevent)/sizeof(struct eventfd));
			}
		}
		currentsocketfd++;
	}

	for(;;)
	{
		int eventcount, k, i;
	
		eventcount = cheeta_event_get(cheeta_thandle, &eventbuffer, tconcurr_per_thread);
		k = eventcount;
		while(k > 0)
		{
			i = k-1;	
			k--;	
			currevent = (*eventbuffer)[i];
			if((currevent->out_event & EPOLLHUP) || (currevent->out_event & EPOLLERR))
			{
				removeevent = currevent;
                cheeta_remove_eventfd(cheeta_thandle, removeevent, 0);
                close(removeevent->fd);
				if(removeevent->ptr)
				{
					free(removeevent->ptr);
					removeevent->ptr = NULL;
				}
				if(removeevent)
					free(removeevent);	
				continue;
			}
			if(currevent->out_event & CH_EV_READ)
			{
				read(currevent->fd, (void *)recvbuffer, recvsize);
				printf("I got %s\n", recvbuffer);
				continue;
			}
			else if(currevent->out_event & CH_EV_WRITE)
			{
				if(!((struct connection *)currevent->ptr)->state)
				{
					int sock_optval = -1;
					int sock_optval_len = sizeof(sock_optval);
					
					if(!getsockopt(currevent->fd, SOL_SOCKET, SO_ERROR, (void *)&sock_optval, (socklen_t *)&sock_optval_len))
					{
						if(!sock_optval)
							((struct connection *)currevent->ptr)->state = CONNECTED;
					} 
				}
				else
				{
					write(currevent->fd, (void *)sendbuffer, strlen(sendbuffer));
				}			
				continue;
			}
		}
		free(eventbuffer);
		eventbuffer = NULL;
	}
	free(socketfds);
}

int main(int argc, char **argv)
{
	int c, ret;
	unsigned int concurrency = 0;
	unsigned int totalnoreq = 0;
	pthread_t threadid;
	pthread_attr_t thread_attr;
	unsigned int configuredcpuscount = 0;
	unsigned int onlinecpuscount = 0;
	unsigned int threadcount = 0;
	struct tcpu_info *ptcpuinfo;
	unsigned int treq_per_thread = 0;
	unsigned int tconcurr_per_thread = 0;
	struct sched_param tsched_param;
    cpu_set_t cpuset;
	char *hostaddr, *hostport;
	
	configuredcpuscount = sysconf(_SC_NPROCESSORS_CONF);
	onlinecpuscount = sysconf(_SC_NPROCESSORS_ONLN);	

	opterr = 0;

	while((c = getopt(argc, argv, "n:c:h:p:")) != -1)
	{
		switch(c)
		{
			case 'c':
				concurrency = atoi(optarg);
				break;
			case 'n':
				totalnoreq = atoi(optarg);
				break;
			case 'h':
				hostaddr = strdup(optarg);
				break;
			case 'p':
				hostport = strdup(optarg);
				break;
			case '?':
				if(optopt == 'c')
					printf("A -c %c was specified without arguments\n", optopt);
				if(optopt == 'n')
					printf("A -n %c was specified without arguments\n", optopt);
				if(optopt == 'h')
					printf("A -h %c was specified without arguments\n", optopt);
				if(optopt == 'p')
					printf("A -p %c was specified without arguments\n", optopt);
				break;
			default:
				abort();
		}	
	}

	if(concurrency == 0 || totalnoreq == 0 || (concurrency > totalnoreq) || ((totalnoreq % concurrency) != 0) )
		abort();	

	printf("Parameters Concurrency\t%d\nTotal Number of Requests\t%d\n", concurrency, totalnoreq);

    threadcount = onlinecpuscount;
	treq_per_thread = totalnoreq/threadcount;
	tconcurr_per_thread = concurrency/threadcount;
    
	tsched_param.sched_priority = sched_get_priority_max(SCHED_RR);

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
        ptcpuinfo->tconcurr_req_thread = tconcurr_per_thread;
        ptcpuinfo->treq_thread = treq_per_thread;
		ptcpuinfo->hostaddr = hostaddr;
		ptcpuinfo->hostport = hostport;

        if((ret = pthread_create(&threadid, &thread_attr, vizsla_client_event_loop, ptcpuinfo)) !=0 )
        {
            printf("pthread_create failed %d\n", ret);
            return ret;
        }

        threadcount--;
    }
	while(1)
		sleep(10);

	return 0;
}
