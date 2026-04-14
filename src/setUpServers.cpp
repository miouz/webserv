#include  "webserv.hpp"

std::vector<ServerConfig> argsToServerConfigs(int ac, char** av)
{
	if (ac != 2)
		throw std::invalid_argument("need 1 config file");
	Config config(av[1]);
	return config.getServers();
}

void	configWebServers(std::vector<ServerConfig>& webserv, Data& data)
{
	data.servers.reserve(1024);
	data.clients.reserve(1024);
	data.fdPool.reserve(1024);
	for (size_t i = 0; i < webserv.size(); i++)
	{
		// if (!webserv[i].isValidServerConfig())
		// 	throw std::runtime_error("config file not valid");
		data.servers.push_back(new Server(webserv[i]));
	}
}

/**
 * @brief this function creat Servers from ServerConfig and setup each server,
 * it adds the correspondant client to clients' pointer vector and pollfd's vector
 *
 * @param data the webserver's data contains servers, clients, pollfd
 * @warning it throws runtime_error
 */
void setUpServers(Data& data)
{
	for (size_t i = 0; i < data.servers.size(); i++)
	{
		data.servers[i]->setUpServer();
		pollfd serverFd = fdToPollfdWithStatus(data.servers[i]->getSockFd(), POLLIN);
		data.fdPool.push_back(serverFd);
	}
	if (data.servers.size() == 0)
		throw std::runtime_error("no server to run");
}
