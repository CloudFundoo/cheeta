#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class client{
public:
	client();
	~client();
	int send(void *, unsigned int);
	int recv(void *, unsigned int &);
	int connect();
private:
	int socketfd;
	struct sockaddr_in instanceaddr;
	static unsigned int instance_cnt;
	struct sockaddr_in remoteaddr;
};
