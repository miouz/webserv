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

	servers.reserve(1024);
	clients.reserve(1024);
	fdPool.reserve(1024);

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
			servers.erase(servers.begin() + i);
			i--;
		}
	}

	if (servers.size() == 0)
	{
		std::cerr << "No server to run\n";
		exit(EXIT_FAILURE);
	}

	//poll the fds to get event
	while (true) {
		int status = poll(fdPool.data(), fdPool.size(), 1000);
		if (status < 0)
		{
			std::cerr << " poll error:" << strerror(errno) << "\n";
			shutDownServers(servers, clients);
		}
		for (int i = 0; i < fdPool.size(); i++)
		{
			if (!fdPool[i].revents)
				continue ;

			int fd = fdPool[i].fd;
			if (fdPool[i].revents & POLLHUP | POLLERR)
				removeClient(findClient(fd));
			//if is server : accept and creat client
			if (isServer(fd))
			{
				Client *newClient = findServer(fd, servers).acceptClient();
				if (newClient)
					clients.push_back(newClient);
			}
			//if is client then handle request or send reply
			else {
				Client *client = findClient(fd);
				if (fdPool[i].revents & POLLIN)
					client->getRequest();
				if (fdPool[i].revents & POLLOUT)
					client->sendReply();
			}
		}
	}
	shutDownServers(servers, clients);
	return EXIT_SUCCESS;
}
