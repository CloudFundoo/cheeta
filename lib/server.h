#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

class server{
public:
        server();
        ~server();
private:
        int socketfd;
        int rsocketfd;
        struct sockaddr_un instanceaddr;
        struct sockaddr_un remoteaddr;
};

