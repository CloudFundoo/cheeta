#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "client.h"
#include "slave.h"


void *vizsla_client_event_loop(void * arg)
{
	struct tcpu_info *ptcpuinfo;
	class client *pclient[] = NULL;

	ptcpuinfo = (struct tcpu_info *)arg;

	pclient = new client();

}

int main(int argc, char **argv)
{
	int c, ret;
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
	unsigned int treq_per_thread = 0;
	unsigned int tconcurr_per_thread = 0;
	struct sched_param tsched_param;
    cpu_set_t cpuset;
	
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
    	CPU_SET(threadcount, &cpuset);
		pthread_attr_setaffinity_np(&thread_attr, sizeof(cpuset), &cpuset);
        ptcpuinfo = (struct tcpu_info *)malloc(sizeof(struct tcpu_info));
        memset(ptcpuinfo, 0, sizeof(struct tcpu_info));
        ptcpuinfo->tconcurr_req_thread = tconcurr_per_thread;
        ptcpuinfo->treq_thread = treq_per_thread;

        if((ret = pthread_create(&threadid, &thread_attr, vizsla_client_event_loop, ptcpuinfo) != 0))
        {
            perror("pthread_create failed");
            return ret;
        }
        threadcount--;
    }

#if 0
	if(!pclient->connect())
	{
		pclient->send((void *)sendbuffer, strlen(sendbuffer));
		pclient->recv((void *)recvbuffer, recvsize);
		printf("I got %s\n", recvbuffer);
	}
	else {
		printf("client::error in connect\n");
	}	
#endif
	return 0;
}
