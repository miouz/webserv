#include "Server.hpp"

Server::Server(ServerConfig& config): sockFd_(-1), config_(config)
{
	clients_.reserve(1024);
}

void Server::setSockFd(int fd){ sockFd_ = fd;}

std::vector<Client*>& Server::getClients(){return clients_;} 

int Server::getSockFd() const { return sockFd_;}

struct sockaddr_in& Server::getAddress() { return address_; }

ServerConfig& Server::getServerConfig() { return config_;}

const std::vector<Location>&	Server::getLocations() const { return config_.getLocations();}

int Server::getListen() { return config_.getListen();}

const std::map<int, std::string>&	Server::getErrorPage() const {return config_.getErrorPage();}

int	Server::getClientMaxBodySize() {return config_.getClientMaxBodySize();}

/**
 * @brief this function closes server's fd 
 * and erase its related clients on vector in order to call the
 * client's destructor.
 */
void Server::closeServer()
{
	for (size_t i = 0; i < clients_.size(); i++)
	{
		delete clients_[i];
		clients_[i] = NULL;
	}
	clients_.clear();
	std::vector<Client*>().swap(clients_);
	// avoid double close
	if (sockFd_ != -1)
	{
		close(sockFd_);
	#ifdef DEBUG
	std::cout << "Server on fd " << sockFd_ << " is shut down\n";
	#endif
		sockFd_ = -1;
	}
}

Server::~Server()
{
	closeServer();
}

/**
 * @brief it creats client and add the client to clinets_ of the server
 *
 * @return address to the new added client who owned by server
 * @warning the function throw exeptions, the ownership of client belongs to its server
 */
Client* Server::acceptClient()
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int fd = accept(sockFd_, (struct sockaddr*)&addr, &len);
	if (fd == RETURN_ERROR)
		throw std::runtime_error(std::string("can't accept new client: ") + std::strerror(errno));
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == RETURN_ERROR
		|| fcntl(fd, F_SETFD, FD_CLOEXEC) == RETURN_ERROR)
	{
		close(fd);
		throw std::runtime_error(std::string("can't set up fd: ") + std::strerror(errno));
	}
	time_t now = std::time(NULL);
	if (now == RETURN_ERROR)
		throw std::runtime_error(std::string("can't set time: ") + std::strerror(errno));
	Client* newCLient = new Client(fd, *this, addr, now);
	clients_.push_back(newCLient);
	#ifdef DEBUG
	std::cout << "\nConnection established on fd " << sockFd_ << ":\n"
		<< *newCLient<<"\n";
	#endif
	return newCLient;
}

/**
 * @brief set up a server with informations inside Server object.
 *
 * @detail it creats a socket, set up the socket , bind it to 
 * the ip address with port, then keep the socket listening for for further connections.
 *
 * @warning this function throw runtime_error in case of any disfunction of syscall
 */
void Server::setUpServer(void)
{
	#ifdef DEBUG
	std::cout << "Setting up server on port " << config_.getListen() << "\n";
	#endif
	int yes = 1;
	sockFd_ = socket(PF_INET, SOCK_STREAM, 0);
	if (sockFd_ == RETURN_ERROR)
		throw std::runtime_error(std::string("socket: ") + strerror(errno));

	if (setsockopt(sockFd_, SOL_SOCKET, SO_REUSEADDR , &yes, sizeof(yes)) == RETURN_ERROR)
		throw std::runtime_error(std::string("set sockopt: ") + strerror(errno));

	if (fcntl(sockFd_, F_SETFL, O_NONBLOCK) == RETURN_ERROR
		|| fcntl(sockFd_, F_SETFD, FD_CLOEXEC) == RETURN_ERROR)
		throw std::runtime_error(std::string("fcntl: ") + strerror(errno));

	address_.sin_family = AF_INET;
	address_.sin_addr.s_addr = INADDR_ANY;
	address_.sin_port = htons(config_.getListen());
	if (bind(sockFd_, (struct sockaddr*)&address_, sizeof(address_)) == RETURN_ERROR)
		throw std::runtime_error(std::string("bind: ") + strerror(errno));


	if (listen(sockFd_, SOMAXCONN) == RETURN_ERROR)
		throw std::runtime_error(std::string("listen: ") + strerror(errno));

	std::cout << *this;
}

/**
 * @brief this function removes a client from the server's clients_ list,
 * it finds the client with its ADDRESS (not fd because the fd could be assigned to a new Client already)
 * it calls the client's destructor by using delete and clients_.erase()
 *
 * @param client reference of the client to remove
 */
void Server::removeClient(Client* client)
{
	if (!client)
		return ;
	for (size_t i = 0; i < clients_.size(); i++)
	{
		if (client == clients_[i])
		{
			delete clients_[i];
			clients_.erase(clients_.begin() + i);
		}
	}
}

std::ostream& operator<<(std::ostream& out, Server& server)
{
	out << "\nServer on fd " << server.getSockFd()
		<< ", listening on port " << server.getListen()
		<< ", connected to " << server.getClients().size() << " clients\n";
	return out;
}

Server* findServer(int fd, std::vector<Server*>& servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		if (fd == servers[i]->getSockFd())
			return servers[i];
	}
	return NULL;
}

pollfd fdToPollfdWithStatus(int fd, short event)
{
	pollfd fdToReturn;

	fdToReturn.fd = fd;
	fdToReturn.events = event;
	fdToReturn.revents = 0;
	return fdToReturn;
}

