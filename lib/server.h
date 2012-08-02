#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class server{
public:
        server();
        ~server();
        int socketfd;
        int rsocketfd;
        struct sockaddr_in instanceaddr;
        struct sockaddr_in remoteaddr;
};

