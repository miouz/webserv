#include "Server.hpp"

/**
 * @brief this function close all fds and free everything before shutting down the server
 */
void Server::closeServer(void)
{
	close(sockFd_);
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


	if (listen(sockFd_, 128) == RETURN_ERROR)
		throw std::runtime_error(std::string("listen: ") + strerror(errno));

	#ifdef DEBUG
	std::cout << "server is set up on fd " << sockFd_ << ", listening on port " << port_ << "\n";
	#endif
}
