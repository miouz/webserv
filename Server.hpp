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

typedef struct Data
{
	std::vector<Server*> servers;
	std::vector<Client*> clients;
	std::vector<pollfd> fdPool;
}	Data;

class Server
{
	int sockFd_;
	int port_;
	sockaddr_in address_;
	std::vector<Client*> clients_;

	public:
	Server();
	Server(int fd, int port);
	~Server();

	//getter and setter
	void setSockFd(int fd);
	void setPort(int port);
	int getSockFd() const;
	sockaddr_in& getAddress();
	int getPort() const;
	std::vector<Client*>& getClients();

	void setUpServer();
	Client* acceptClient();
	void removeClient(Client* client);
	void closeServer();
};

std::ostream& operator<<(std::ostream& out, Server& server);
Server* findServer(int fd, std::vector<Server*>& servers);
pollfd fdToPollfdWithStatus(int fd, short event);
void	shutDownServers(std::vector<Server*>& servers);
void setUpServers(Data& data);
void pollEventsLoop(Data& data);
#endif // !SERVER_HPP
