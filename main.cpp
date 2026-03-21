#include "Server.hpp"
#include "Client.hpp"
#include <algorithm>
#include <vector>

#define SERVER_NUM 1

int main()
{
	std::vector<Server> servers;
	std::vector<Client*> clients;
	std::vector<pollfd> fdPool;

	//TODO: parser .config and setup each server by calling server's constructor
	Server server[SERVER_NUM];
	servers.insert(servers.begin(), &server[0], &servers[SERVER_NUM]);
	
	//set up each server
	for (int i = 0; i < SERVER_NUM; i++)
	{
		try {
		servers[i].setUpServer();
		pollfd serverFd = fdToPollfdWithStatus(servers[i].getSockFd(), POLLIN);
		fdPool.push_back(serverFd);
		}
		catch(std::exception& e){
			std::cerr << "cant set up server:" << e.what() << "\n";
			servers[i].closeServer();
			servers.erase(servers.begin() + i);
		}
	}


	return EXIT_SUCCESS;
}
