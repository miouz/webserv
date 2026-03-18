#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>

#define RETURN_ERROR -1

class Server
{
	int sockFd_;
	int port_;
	std::vector<pollfd> fds;
	sockaddr_in address_;

	public:
	Server(){}
	Server(int fd, int port);

	//getter and setters
	void setSockFd(int fd);
	void setPort(int port);
	int getSockFd() const;
	struct sockaddr_in getAddress() const;
	int getPort() const;

	void setUpServer();
	void closeServer();
};

