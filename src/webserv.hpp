#ifndef WEBSERV_HPP 
#define WEBSERV_HPP

#define READ_BUFF_SIZE 1024
#define POLL_TIMEOUT 1000
#define SERVER_NUM 1


#include "Server.hpp"
#include "Client.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "RequestParser.hpp"
#include <algorithm>
#include <vector>
#include <sys/types.h>

enum HandlerResult
{
	HANDLER_OK,
	HANDLER_DISCONNECT,
};

typedef struct Data
{
	std::vector<Server*>	servers;
	std::vector<Client*>	clients;
	std::vector<pollfd>		fdPool;
}	Data;

std::vector<ServerConfig>	argsToServerConfigs(int ac, char** av);
void						configWebServers(std::vector<ServerConfig>& webserv, Data& data);
void						setUpServers(Data& data);
void						pollEventsLoop(Data& data);
void						shutDownServers(std::vector<Server*>& servers);

#endif // !WEBSERV_HPP
