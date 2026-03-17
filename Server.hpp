#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>

#define RETURN_ERROR -1

class Server
{
	int sockFd_;
	int port_;
	struct sockaddr_in address_;

	public:
	Server(){}
	Server(int fd, int port): sockFd_(fd), port_(port){}

	//getter and setters
	void setSockFd(int fd){ sockFd_ = fd;}
	void setPort(int port){ port_ = port;}
	int getSockFd() const { return sockFd_;}
	struct sockaddr_in getAddress() const { return address_; }
	int getPort() const { return port_; }

	void setUpServer();
	void closeServer();
};

