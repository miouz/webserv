#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <csignal>
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
#include "GetRequest.hpp"

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

enum cgiFD
{
	READ,
	WRITE
};

class Client
{
	int				fd_;
	int				cgiFd_[2];
	pid_t			cgiPid_;
	Server&			server_;
	sockaddr_in		address_;
	std::string		bufferOut_;
	size_t			sent_;
	size_t			written_;
	clientStatus	status_;
	time_t			lastActivityTime_;
	RequestParser	parsedRequest_;

	
	
	public:
	Client(int fd, Server& server, sockaddr_in& address, time_t& creationTime);
	~Client();

	//getters and setters
	int getFd() const;
	int* getCgiFd();
	pid_t getCgiPid() const;
	void setCgiPid(pid_t);
	sockaddr_in& getAddress();
	clientStatus getStatus() const;
	Server& getServer() const;
	void setStatus(clientStatus status);
	std::string& getBufferOut();
	time_t	getLastActivityTime() const;
	RequestParser& getParsedRequest();

	void closeCgiReadFd();
	void closeCgiWriteFd();
	void killCgi();
	void closeClient();
	void removeFromServer();
	bool isTimeOut();
	void updateLastActivityTime();
	void buildResponse();
	void addCgiFdsToPool(std::vector<pollfd>& fdPool);
	bool sendResponse();
	bool cgiWriteBody();
};

std::ostream& operator<<(std::ostream& out, Client& client);
Client* findClient(int fd, std::vector<Client*>& clients);
void	disconnectClient(int fd, std::vector<Client*>& clients,
					  std::vector<pollfd>& fdPool, std::vector<pollfd>::iterator toRemove);
bool	isCgi(std::string& uri, Location& location);
#endif // !CLIENT_HPP
