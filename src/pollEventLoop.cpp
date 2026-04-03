#include "webserv.hpp"

/**
 * @brief it loops on each Client to check if is timeout, then try to disconnect them and free their resources
 * @waning it catches each disconnect error locally in order to continue on next Client
 *
 * @param data the Data sturcture
 */
void cleanTimeOutClient(Data& data)
{
	for (size_t i = 0; i < data.clients.size(); i++)
	{
		if (data.clients[i]->isTimeOut())
		{
			#ifdef DEBUG
			std::cout << "\nClient on fd " << data.clients[i]->getFd() 
							<< " is time out, disconnecting from server:\n";
			#endif
			try{
				int clientFd = data.clients[i]->getFd();
				std::vector<pollfd>::iterator toRemove = data.fdPool.end();
				for(size_t i = 0; i < data.fdPool.size(); i++)
				{
					if (clientFd == data.fdPool[i].fd)
					{
						toRemove = data.fdPool.begin() + i;
						break ;
					}	
				}
				disconnectClient(clientFd, data.clients, data.fdPool, toRemove);
				i--;
	 		 }catch(std::exception& e){
				std::cerr<<"Error: can't disconnect client: "<<e.what() << ", continue\n";
			}
		}
	}
}

void clientEventHandler(Data& data, pollfd& fd, Client* client)
{
	(void)data;
	char buff[READ_BUFF_SIZE] = {0};

	if (fd.revents & POLLIN)
	{
		ssize_t bytesRead = read(client->getFd(), buff, READ_BUFF_SIZE);

		if(bytesRead > 0) 
		{
			#ifdef DEBUG
				std::cout << "\nClient on fd "<<client->getFd() <<  " recieved request:\n" << buff << "\n";
			#endif
		}
	}
	if (fd.revents & POLLOUT)
	{
		#ifdef DEBUG
		std::cout << "ready to send response\n";
		#endif
	}
}

void serverEventHandler(Data& data, pollfd& fd)
{
	Server* server = findServer(fd.fd, data.servers);
	if (server)
	{
		Client* client = server->acceptClient();
		data.clients.push_back(client);
		pollfd clientFd = fdToPollfdWithStatus(client->getFd(), POLLIN);
		data.fdPool.push_back(clientFd);
	}
}

/**
 * @brief this fundtion loop on the fdPool to handle events arrived on a particular pollfd
 *
 * @param data the Data structure
 * @warning this function is nothrow, it catches error in his level and just continue
 * so that all Servers continue to work
 */
void eventsHandler(Data& data)
{
	for (size_t i = 0; i < data.fdPool.size(); i++)
	{
		if (!data.fdPool[i].revents)
			continue ;

		if (data.fdPool[i].revents & (POLLHUP | POLLERR))
		{
			try{
			disconnectClient(data.fdPool[i].fd, data.clients, data.fdPool, data.fdPool.begin() + i);
			i--;
	  		}catch(std::exception& e) {
	  			std::cerr<<"Error: cant disconnect client: "<< e.what()<<", continue\n";
			}
			continue;
		}
		Client *client = findClient(data.fdPool[i].fd, data.clients);
		try {
			//if is client
			if (client)
				clientEventHandler(data, data.fdPool[i], client);
			//if is server
			else
				serverEventHandler(data, data.fdPool[i]);
		} catch (std::exception& e) {
			std::cerr << "Error: events handler: " << e.what() << ", continue\n";
		}
	}
}

/**
 * @brief the core poll() loop who watch the fdPool and handle event by evnet:
 * if poll() syscall fails: EXIT program
 * if poll() reach timeout: check Client timeout and clean them
 * if poll() returns N means N fd got events, then loop on the fds to handle
 *
 * @param data the Data structure who contains active Servers Clients, and fdPool informations
 * @warning this function EXIT and nothrow 
*/
void pollEventsLoop(Data& data)
{
	while (true) {
		#ifdef DEBUG
		std::cout << "\nfdPool to watch:";
		for (size_t i = 0; i<data.fdPool.size();i++)
			std::cout << data.fdPool[i].fd << "  ";
		std::cout << "\n";
		#endif
		int status = poll(data.fdPool.data(), data.fdPool.size(), POLL_TIMEOUT);
		if (status < 0)
		{
			std::cerr << "Fatal Error: poll():" << strerror(errno) << ", exit\n";
			shutDownServers(data.servers);
			exit(EXIT_FAILURE);
		}
		if (status == 0)
		{
			cleanTimeOutClient(data);
			continue;
		}
		if (status > 0)
			eventsHandler(data);
	}
}
