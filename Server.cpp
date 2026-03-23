#include "Server.hpp"

Server::Server(): sockFd_(-1), port_(-1){clients_.reserve(1024);}

Server::Server(int fd, int port): sockFd_(fd), port_(port){clients_.reserve(1024);}

void Server::setSockFd(int fd){ sockFd_ = fd;}

void Server::setPort(int port){ port_ = port;}

int Server::getSockFd() const { return sockFd_;}

struct sockaddr_in& Server::getAddress() { return address_; }

int Server::getPort() const { return port_; }

/**
 * @brief this function closes server's fd 
 * and erase its related clients on vector in order to call the
 * client's destructor.
 */
void Server::closeServer()
{
	// avoid double close
	if (sockFd_ != -1)
	{
		close(sockFd_);
	#ifdef DEBUG
	std::cout << "Server on fd " << sockFd_ << " is down\n";
	#endif
		sockFd_ = -1;
	}
	//call its clients' destructors
	clients_.clear();
}

Server::~Server()
{
	closeServer();
}

/**
 * @brief it creats client and add the client to clinets_ of the server
 *
 * @return address to the new added client who owned by server
 * @warning the ownership of client belongs to its server
 */
Client* Server::acceptClient()
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int fd = accept(sockFd_, (struct sockaddr*)&addr, &len);
	if (fd == RETURN_ERROR)
	{
		std::cerr << "Error: can't accept new client:" << std::strerror(errno) << "\n";
		return NULL;
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == RETURN_ERROR
		|| fcntl(fd, F_SETFD, FD_CLOEXEC) == RETURN_ERROR)
	{
		close(fd);
		return NULL;
	}
	clients_.emplace_back(fd, *this, addr);
	return &clients_.back();
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
	address_.sin_port = htons(port_);
	if (bind(sockFd_, (struct sockaddr*)&address_, sizeof(address_)) == RETURN_ERROR)
		throw std::runtime_error(std::string("bind: ") + strerror(errno));


	if (listen(sockFd_, SOMAXCONN) == RETURN_ERROR)
		throw std::runtime_error(std::string("listen: ") + strerror(errno));

	#ifdef DEBUG
	std::cout << "server is set up on fd " << sockFd_ << ", listening on port " << port_ << "\n";
	#endif
}


/**
 * @brief this function removes a client from the server's clients_ list,
 * it calls the client's destructor by using clients_[i].erase()
 *
 * @param client reference of the client to remove
 */
void Server::removeClient(Client& client)
{
	for (int i = 0; i < clients_.size(); i++)
	{
		if (client.getFd() == clients_[i].getFd())
			clients_.erase(clients_.begin() + i);
	}
}

bool	isServer(int fd, std::vector<Server>& servers)
{
	for (int i = 0; i < servers.size(); i++)
	{
		if (fd == servers[i].getSockFd())
			return true;
	}
	return false;
}

Server* findServer(int fd, std::vector<Server>& servers)
{
	for (int i = 0; i < servers.size(); i++)
	{
		if (fd == servers[i].getSockFd())
			return &(servers[i]);
	}
	return NULL;
}

void	shutDownServers(std::vector<Server>& servers)
{
	servers.clear();
}
