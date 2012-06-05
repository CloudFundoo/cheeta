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
        struct sockaddr_un instanceaddr;
        struct sockaddr_un remoteaddr;
};

