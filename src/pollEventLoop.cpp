#include "webserv.hpp"
#include <sys/types.h>
#include <sys/wait.h>


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

/**
 * @brief [TODO: this function reads partially the result of cgi execution
 *          and feed the buffer in order to generate the response]
 *
 * @param data [struct Data]
 * @param fd event's pollfd
 * @param client pointer to Client
 * @return return HANDLER_OK to stay connected (NO WAY TO DISCONNECT)
 */	
HandlerResult clientCgiReadHandler(Data& data, pollfd& fd, Client* client)
{
	char buff[READ_BUFF_SIZE] = {0};
	ssize_t bytesRead = read(fd.fd, buff, READ_BUFF_SIZE);


#ifdef DEBUG
	std::cerr << "clientCgiReadHandler\n";
	std::cerr << "bytesRead =" << bytesRead << '\n';
#endif
	//TODO: execution of cgi finished
	if (bytesRead > 0)
		client->getRequest().getData().body.append(buff, bytesRead);

	
	int exitStatus;
	//if == 0 is error so let client time out disconnect it
	if (waitpid(client->getCgiPid(), &exitStatus, WNOHANG) == 0)
	{
		std::cerr << "STILL waitpiding\n";
		return HANDLER_OK;
	}
	else
		std::cerr << "Done waiting\n";

	client->closeCgiReadFd();
	std::vector<pollfd>::iterator cgiReadFdInPool = findFdInPool(data.fdPool, fd.fd);
	data.fdPool.erase(cgiReadFdInPool);

	//TODO: if error put the exitstatus code on request's error code
#ifdef DEBUG
	std::cerr << "Wexitstatus= " << WEXITSTATUS(exitStatus) << '\n';
	std::cerr << "exitstatus= " << exitStatus << '\n';
#endif


	if ((WIFEXITED(exitStatus) && WEXITSTATUS(exitStatus) == 1) || exitStatus == 500)
		client->getRequest().getData().code = 500;
	else
		client->getRequest().getData().code = 200;

	//TODO: generate response
#ifdef DEBUG
	std::cerr << "cgi finished send response\n";
#endif
	client->buildResponse();
	//MAYBE

	std::vector<pollfd>::iterator clientFdInPool = findFdInPool(data.fdPool, client->getFd());
	if (clientFdInPool == data.fdPool.end())
		return HANDLER_DISCONNECT;
	data.fdPool[clientFdInPool - data.fdPool.begin()].events = POLLOUT;
	#ifdef DEBUG
	std::cerr << "POLLOUT\n";
#endif
	return HANDLER_OK;
}

HandlerResult clientCgiWriteHandler(Data& data, pollfd& fd, Client* client)
{
	bool fullyWritten = client->cgiWriteBody();

#ifdef DEBUG
	std::cerr << "clientCgiWriteHandler\n";
#endif
	if(fullyWritten == true)
	{
		client->closeCgiWriteFd();
		std::vector<pollfd>::iterator cgiWriteFdInPool = findFdInPool(data.fdPool, fd.fd);
		data.fdPool.erase(cgiWriteFdInPool);

#ifdef DEBUG
		std::cerr << "client fully written on pipe fd " << fd.fd << "\n";
#endif
	}
	return HANDLER_OK;
}

HandlerResult clientRecieveHandler(Data& data, pollfd& fd, Client* client)
{
	char buff[READ_BUFF_SIZE] = {0};

#ifdef DEBUG
	std::cerr << "clientRecieveHandler\n";
#endif
	ssize_t bytesRead = recv(client->getFd(), buff, READ_BUFF_SIZE, 0);
	if (bytesRead == 0)
		return HANDLER_DISCONNECT;
	if (bytesRead > 0)
	{
		std::string toFeed(buff, bytesRead);
		client->getRequest().feed(toFeed);
		if (client->getRequest().isComplete())
		{
			std::string	resolvedUri = client->getServer().getServerConfig().resolvePath(client->getRequest().getData().uri);
			if (resolvedUri == "/..")
				client->getRequest().getData().code = 403;
			Location location = client->getServer().getServerConfig().findLocation(client->getRequest().getData().uri);
			if (location.getPath().empty())
				client->getRequest().getData().code = 403;
#ifdef DEBUG
			std::cerr << "resolvePath:" << resolvedUri << '\n';
#endif
			if ( Request::isCgi(client->getRequest().getData().uri, location) == true)
			{
				client->getRequest().getData().isCgi = true;
				Cgi cgi(client->getServer().getServerConfig(), client->getRequest().getData());

#ifdef DEBUG
				std::cerr<< " begin cgi execution\n";
#endif
				int result = cgi.execute(*client, data.fdPool);
				client->getRequest().getData().code = result;
			}
			if (client->getRequest().getData().isCgi == false
				|| client->getRequest().getData().code != 200)
			{
				client->buildResponse();
				fd.events = POLLOUT;
			}
#ifdef DEBUG
			std::cerr << "\nClient on fd "<<client->getFd() <<  " recieved request:\n" << buff << "\n";
#endif
		}
	}
	return HANDLER_OK;
}

HandlerResult clientSendHandler(Client* client)
{
#ifdef debug
	std::cout << "clientSendHandler\n";
#endif
	bool fullySent = client->sendResponse();
	if (fullySent)
	{
#ifdef DEBUG
		std::cerr << "response fully sent\n";
#endif
		return HANDLER_DISCONNECT;
	}
	return HANDLER_OK;
}

/**
 * @brief handle client's POLLIN POLLHUP events
 * for POLLIN: bytesRead = 0 means client disconnected , need to clean up resources
 * bytesRead < 0 not error because of the non blocking modle
 *
 * @param data Data structur of program
 * @param fd fdPool of program
 * @param client pointer contains address of Client
 * @return return HandlerResult: HANDLER_OK or HANDLER_DISCONNECT
 */
HandlerResult clientEventHandler(Data& data, pollfd& fd, Client* client)
{
	(void)data;
	HandlerResult result = HANDLER_OK;

	if (fd.revents & POLLIN)
	{
		if (fd.fd == client->getCgiFd()[READ])
			result = clientCgiReadHandler(data, fd, client);
		else
			result = clientRecieveHandler(data, fd, client);
	}
	if (fd.revents & POLLOUT)
	{
		if (fd.fd == client->getCgiFd()[WRITE])
			result = clientCgiWriteHandler(data, fd, client);
		else
			result = clientSendHandler(client);
	}
	return result;
}

HandlerResult serverEventHandler(Data& data, pollfd& fd)
{
	Server* server = findServer(fd.fd, data.servers);
	if (server)
	{
		while (1)
		{
			Client* client = server->acceptClient();
			//accept() return -1 but not error
			if (client == NULL)
				return HANDLER_OK;
			data.clients.push_back(client);
			pollfd clientFd = fdToPollfdWithStatus(client->getFd(), POLLIN);
			data.fdPool.push_back(clientFd);
		}
	}
	return HANDLER_DISCONNECT;
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

		try {
			Client *client = findClient(data.fdPool[i].fd, data.clients);

			if (client)
			{
				bool isCgiReadFd = (data.fdPool[i].fd == client->getCgiFd()[READ]);
				bool shouldDisconnect = false;

				if (data.fdPool[i].revents & (POLLHUP | POLLERR))
				{
					if (isCgiReadFd)
						clientCgiReadHandler(data, data.fdPool[i], client); // lire le reste + envoyer
					else
						shouldDisconnect = true;
				}
				else if (clientEventHandler(data, data.fdPool[i], client) == HANDLER_DISCONNECT)
					shouldDisconnect = true;
				if (shouldDisconnect == true)
				{
					disconnectClient(data.fdPool[i].fd, data.clients, data.fdPool, data.fdPool.begin() + i);
					--i;
					continue ;
				}
			}
			else
			{
				if (data.fdPool[i].revents & (POLLHUP | POLLERR)
					|| serverEventHandler(data, data.fdPool[i]) == HANDLER_DISCONNECT)
				{
					shutDownServers(data.servers);
					std::vector<pollfd>().swap(data.fdPool);
					std::vector<Client*>().swap(data.clients);
					exit(EXIT_FAILURE);
				}
			}

		} catch (std::exception& e) {
			std::cerr<<"Error: event handler: " << e.what() << "\n";
			continue ;
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
		std::cerr << "\nfdPool to watch:";
		for (size_t i = 0; i<data.fdPool.size();i++)
			std::cerr << data.fdPool[i].fd << "  ";
		std::cerr << "\n";
#endif
		int status = poll(data.fdPool.data(), data.fdPool.size(), POLL_TIMEOUT);
		if (status < 0)
		{
			if (gStop == true || errno == EINTR)
				std::cout << "\nCleaning resources and exit\n";
			else
				std::cerr << "Fatal Error: poll():" << strerror(errno) << ", exit\n";
			shutDownServers(data.servers);
			std::vector<pollfd>().swap(data.fdPool);
			std::vector<Client*>().swap(data.clients);
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
