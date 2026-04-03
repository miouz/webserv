#ifndef CORE_HPP 
#define CORE_HPP

#define READ_BUFF_SIZE 1024
#define POLL_TIMEOUT 1000
#define SERVER_NUM 1

#include "Server.hpp"
#include "Client.hpp"
#include <algorithm>
#include <vector>
#include <sys/types.h>

typedef struct Data
{
	std::vector<Server*> servers;
	std::vector<Client*> clients;
	std::vector<pollfd> fdPool;
}	Data;

void	setUpServers(Data& data);
void	pollEventsLoop(Data& data);
void	shutDownServers(std::vector<Server*>& servers);

#endif // !CORE_HPP
