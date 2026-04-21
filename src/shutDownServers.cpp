#include "webserv.hpp"

void	shutDownServers(std::vector<Server*>& servers)
{
	for (size_t i = 0; i < servers.size(); i++)
		delete servers[i];
	servers.clear();
}
