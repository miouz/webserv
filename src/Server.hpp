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
#include "ServerConfig.hpp"
#include <netinet/in.h>
#include <vector>


class Server
{
	int						sockFd_;
	sockaddr_in				address_;
	std::vector<Client*>	clients_;
	ServerConfig			config_;

	public:
	Server(ServerConfig& config);
	Server(int fd, int port);
	~Server();

	//getter and setter
	void						setSockFd(int fd);
	void						setPort(int port);
	int							getSockFd() const;
	sockaddr_in&				getAddress();
	int							getPort() const;
	std::vector<Client*>& 		getClients();
	ServerConfig				getServerConfig() const;

	//wraper ServerConfig
	std::vector<Location>		getLocations();
	int							getListen();
	std::map<int, std::string>	getErrorPage();
	int							getClientMaxBodySize();

	void setUpServer();
	Client* acceptClient();
	void removeClient(Client* client);
	void closeServer();
};

std::ostream& operator<<(std::ostream& out, Server& server);
Server* findServer(int fd, std::vector<Server*>& servers);
pollfd fdToPollfdWithStatus(int fd, short event);
#endif // !SERVER_HPP
