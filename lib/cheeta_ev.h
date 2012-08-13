#include <sys/epoll.h>

struct eventfd{
	int fd;
	void *ptr;
	unsigned int in_event;
	unsigned int out_event;	
};

/** FD set **/
struct cheeta_context {
	int epfd;
	unsigned int polledfdscount;
};


#define CH_EV_READ	EPOLLIN
#define CH_EV_WRITE	EPOLLOUT

struct cheeta_context *cheeta_event_init(void);
int cheeta_event_get(struct cheeta_context *, struct eventfd *(**)[1], unsigned int);
unsigned int cheeta_add_eventfd(struct cheeta_context *, struct eventfd *, unsigned int);

unsigned int cheeta_remove_eventfd(struct cheeta_context *, struct eventfd *, unsigned int);

unsigned int cheeta_modify_eventfd(struct cheeta_context *, struct eventfd *, unsigned int);
