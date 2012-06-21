#include <sys/epoll.h>

struct eventfd{
	int fd;
	unsigned int in_event;
	unsigned int out_event;	
};

struct cheeta_session{
	int remotefd;
	char buffer[4092];
	int requestsize;
	int responsesize;
	int ready4write;
	int writesize;
};

/** FD set **/
struct cheeta_context {
	int epfd;
	struct epoll_event onevent[32];	
	struct cheeta_session sessions[32];
};


#define CH_EV_READ	EPOLLIN
#define CH_EV_WRITE	EPOLLOUT

struct cheeta_context *cheeta_event_init(void);
unsigned int cheeta_event_get(struct cheeta_context *, struct eventfd *, unsigned int);
unsigned int cheeta_add_eventfd(struct cheeta_context *, struct eventfd *, unsigned int);

inline unsigned int cheeta_remove_eventfd(struct cheeta_context *, struct eventfd *, unsigned int);

inline unsigned int cheeta_modify_eventfd(struct cheeta_context *, struct eventfd *, unsigned int);
