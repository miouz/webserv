#ifndef SERVER_HPP
#define SERVER_HPP

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include "Client.hpp"
#include <netinet/in.h>

class Server
{
	int sockFd_;
	int port_;
	sockaddr_in address_;
	std::vector<Client> clients_;

	public:
	Server();
	Server(int fd, int port);
	Server& operator=(const Server& other);
	~Server();

	//getter and setter
	void setSockFd(int fd);
	void setPort(int port);
	int getSockFd() const;
	sockaddr_in& getAddress();
	int getPort() const;
	std::vector<Client> getClients() const;

	void setUpServer();
	Client* acceptClient();
	void removeClient(Client& client);
	void closeServer();
};

std::ostream& operator<<(std::ostream& out, Server& server);
bool	isServer(int fd, std::vector<Server>& servers);
Server* findServer(int fd, std::vector<Server>& servers);
pollfd fdToPollfd(int fd);
pollfd fdToPollfdWithStatus(int fd, int status);
void	shutDownServers(std::vector<Server>& servers);
#endif // !SERVER_HPP
