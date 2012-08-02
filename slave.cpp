#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "client.h"
#include "vizsla.h"


void *vizsla_client_event_loop(void * arg)
{
	struct sched_param tsched_param;
    cpu_set_t cpuset;
	struct tcpu_info *ptcpuinfo;

	ptcpuinfo = (struct tcpu_info *)arg;
    tsched_param.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_setschedparam(pthread_self(), SCHED_RR, &tsched_param);

    CPU_ZERO(&cpuset);
    CPU_SET(ptcpuinfo->cpu_num, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset); 
    pthread_yield();

}

int main(int argc, char **argv)
{
	int c, ret;
	class client *pclient = new client();
	char sendbuffer[100] = "I am fucked and you?";
	char recvbuffer[100];
	unsigned int recvsize = 100;
	unsigned int concurrency = 0;
	unsigned int totalnoreq = 0;
	pthread_t threadid;
	pthread_attr_t thread_attr;
	unsigned int configuredcpuscount = 0;
	unsigned int onlinecpuscount = 0;
	unsigned int threadcount = 0;
	struct tcpu_info *ptcpuinfo;
	
	configuredcpuscount = sysconf(_SC_NPROCESSORS_CONF);
	onlinecpuscount = sysconf(_SC_NPROCESSORS_ONLN);	

	opterr = 0;

	while((c = getopt(argc, argv, "n:c:")) != -1)
	{
		switch(c)
		{
			case 'c':
				concurrency = atoi(optarg);
				break;
			case 'n':
				totalnoreq = atoi(optarg);
				break;
			case '?':
				if(optopt == 'c')
					printf("A -c %c was specified without arguments\n", optopt);
				if(optopt == 'n')
					printf("A -n %c was specified without arguments\n", optopt);
				break;
			default:
				abort();
		}	
	}

	if(concurrency == 0 || totalnoreq == 0)
		abort();	

    threadcount = onlinecpuscount;

	while(threadcount){
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        ptcpuinfo = (struct tcpu_info *)malloc(sizeof(struct tcpu_info));
        memset(ptcpuinfo, 0, sizeof(struct tcpu_info));
        ptcpuinfo->cpu_num = threadcount;

        if((ret = pthread_create(&threadid, &thread_attr, vizsla_client_event_loop, ptcpuinfo) != 0))
        {
            perror("pthread_create failed");
            return ret;
        }
        threadcount--;
    }

	printf("Parameters Concurrency\t%d\nTotal Number of Requests\t%d\n", concurrency, totalnoreq);

	if(!pclient->connect())
	{
		pclient->send((void *)sendbuffer, strlen(sendbuffer));
		pclient->recv((void *)recvbuffer, recvsize);
		printf("I got %s\n", recvbuffer);
	}
	else {
		printf("client::error in connect\n");
	}	

	return 0;
}
