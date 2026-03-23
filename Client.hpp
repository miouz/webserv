#ifndef CLIENT_HPP
#define CLIENT_HPP

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

class Server;

enum clientStatus
{
	CONNECTED,
	REQUEST_COMPLETE,
	RESPONSE_READY,
	DISCONNECT
};

class Client
{
	int fd_;
	Server& server_;
	sockaddr_in address_;
	std::string bufferIn_;
	std::string bufferOut_;
	clientStatus status_;
	
	public:
	Client(int fd, Server& server, sockaddr_in& address);
	~Client();
	Client(const Client& other);

	//getters and setters
	int getFd() const;
	sockaddr_in& getAddress();
	clientStatus getStatus() const;
	Server& getServer() const;
	void setStatus(clientStatus status);
	std::string& getBufferIn();
	std::string& getBufferOut();

	void closeClient();
};
#endif // !CLIENT_HPP
