#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cerrno>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <ctime>
#include <vector>
#include "RequestParser.hpp"

#define RETURN_ERROR -1
#define CLIENT_TIMEOUT_SECONDES 30

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
	int				fd_;
	Server&			server_;
	sockaddr_in		address_;
	std::string		bufferOut_;
	size_t			sent_;
	clientStatus	status_;
	time_t			lastActivityTime_;
	RequestParser	request_;

	
	
	public:
	Client(int fd, Server& server, sockaddr_in& address, time_t& creationTime);
	~Client();
	Client(const Client& other);

	//getters and setters
	int getFd() const;
	sockaddr_in& getAddress();
	clientStatus getStatus() const;
	Server& getServer() const;
	void setStatus(clientStatus status);
	std::string& getBufferOut();
	time_t	getLastActivityTime() const;
	RequestParser& getRequest();

	void closeClient();
	void removeFromServer();
	bool isTimeOut();
	void updateLastActivityTime();
	void buildResponse();
	bool sendResponse();
};

std::ostream& operator<<(std::ostream& out, Client& client);
Client* findClient(int fd, std::vector<Client*>& clients);
void	disconnectClient(int fd, std::vector<Client*>& clients,
					  std::vector<pollfd>& fdPool, std::vector<pollfd>::iterator toRemove);
#endif // !CLIENT_HPP
