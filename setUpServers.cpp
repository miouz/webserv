#include  "core.hpp"

/**
 * @brief this function creat Servers from ServerConfig and setup each server,
 * it adds the correspondant client to clients' pointer vector and pollfd's vector
 *
 * @param data the webserver's data contains servers, clients, pollfd
 * @warning it exits if there is no Server to be run with EXIT_FAILURE
 */
void setUpServers(Data& data)
{
	for (int i = 0; i < SERVER_NUM; i++)
	{
		try {
		//TODO: change this part by transforming ServerConfig class to Server by calling Server's constructor
		data.servers.push_back(NULL);
		data.servers[i] = new Server(-1, 8080);
		data.servers[i]->setUpServer();
		pollfd serverFd = fdToPollfdWithStatus(data.servers[i]->getSockFd(), POLLIN);
		data.fdPool.push_back(serverFd);
		}
		catch(std::exception& e){
			std::cerr << "cant set up server " << i << " : "<< e.what() << ", continue\n";
			delete data.servers[i];
			data.servers.erase(data.servers.begin() + i);
			i--;
		}
	}
	if (data.servers.size() == 0)
	{
		std::cerr << "No server to run\n";
		exit(EXIT_FAILURE);
	}
}
