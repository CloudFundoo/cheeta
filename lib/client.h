#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

class client{
public:
	client();
	~client();
	int send(void *, unsigned int);
	int recv(void *, unsigned int &);
	int connect();
private:
	int socketfd;
	struct sockaddr_un instanceaddr;
	static unsigned int instance_cnt;
	struct sockaddr_un remoteaddr;
};
