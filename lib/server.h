#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

class server{
public:
        server();
        ~server();
        int send(void *, unsigned int);
        int recv(void *, unsigned int &);
        int connect(void);
	int waitforclient();
private:
        int socketfd;
        int rsocketfd;
        struct sockaddr_un instanceaddr;
        struct sockaddr_un remoteaddr;
};

