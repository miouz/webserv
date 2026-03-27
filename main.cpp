#include "Server.hpp"
#include "Client.hpp"
#include <algorithm>
#include <vector>
#include <sys/types.h>

#define SERVER_NUM 1
#define READ_BUFF_SIZE 1024
#define POLL_TIMEOUT 1000

int main()
{
	std::vector<Server*> servers;
	std::vector<Client*> clients;
	std::vector<pollfd> fdPool;

	servers.reserve(1024);
	clients.reserve(1024);
	fdPool.reserve(1024);

	//TODO: parser .config and setup each server by calling server's constructor


	//set up each server
	for (int i = 0; i < SERVER_NUM; i++)
	{
		try {
		servers.push_back(NULL);
		servers[i] = new Server(-1, 8080);
		servers[i]->setUpServer();
		pollfd serverFd = fdToPollfdWithStatus(servers[i]->getSockFd(), POLLIN);
		fdPool.push_back(serverFd);
		}
		catch(std::exception& e){
			std::cerr << "cant set up server " << i << " : "<< e.what() << "\n";
			delete servers[i];
			servers.erase(servers.begin() + i);
			i--;
		}
	}

	if (servers.size() == 0)
	{
		std::cerr << "No server to run\n";
		exit(EXIT_FAILURE);
	}

	char buff[READ_BUFF_SIZE] = {0};
	//poll the fds to get event
	while (true) {
		#ifdef DEBUG
		std::cout << "\nfdPool to watch:";
		for (size_t i = 0; i<fdPool.size();i++)
			std::cout << fdPool[i].fd << "  ";
		std::cout << "\n";
		#endif
		int status = poll(fdPool.data(), fdPool.size(), POLL_TIMEOUT);
		if (status < 0)
		{
			std::cerr << " poll error:" << strerror(errno) << "\n";
			shutDownServers(servers);
			exit(EXIT_FAILURE);
		}
		if (status == 0)
		{
			for (size_t i = 0; i < clients.size(); i++)
			{
				if (clients[i]->isTimeOut())
				{
					#ifdef DEBUG
					std::cout << "\nClient on fd " << clients[i]->getFd() 
									<< " is time out, disconnecting from server:\n";
					#endif
					int clientFd = clients[i]->getFd();
					std::vector<pollfd>::iterator toRemove = fdPool.end();
					for(size_t i = 0; i < fdPool.size(); i++)
					{
						if (clientFd == fdPool[i].fd)
						{
							toRemove = fdPool.begin() + i;
							break ;
						}	
					}
					disconnectClient(clientFd, clients, fdPool, toRemove);
					i--;
				}
			}
			continue;
		}
		for (size_t i = 0; i < fdPool.size(); i++)
		{
			if (!fdPool[i].revents)
				continue ;

			int fd = fdPool[i].fd;
			if (fdPool[i].revents & (POLLHUP | POLLERR))
			{
				disconnectClient(fd, clients, fdPool, fdPool.begin() + i);
				i--;
				continue;
			}
			Client *client = findClient(fd, clients);
			//if is client
			if (client)
			{
				if (fdPool[i].revents & POLLIN)
				{
					ssize_t bytesRead = read(client->getFd(), buff, READ_BUFF_SIZE);

					if(bytesRead > 0) 
					{
						#ifdef DEBUG
							std::cout << "\nClient on fd "<<client->getFd() <<  " recieved request:\n" << buff << "\n";
						#endif
					}
				}
				if (fdPool[i].revents & POLLOUT)
				{
					#ifdef DEBUG
					std::cout << "ready to send response\n";
					#endif
				}

			}
			//if is server
			else {
				Server* server = findServer(fd, servers);
				if (server)
					client = server->acceptClient();
				if (client)
				{
					clients.push_back(client);
					pollfd clientFd = fdToPollfdWithStatus(client->getFd(), POLLIN);
					fdPool.push_back(clientFd);
				}
			}
		}
	}
	shutDownServers(servers);
	return EXIT_SUCCESS;
}
