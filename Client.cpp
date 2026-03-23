#include "Client.hpp"
#include "Server.hpp"

/**
 * @brief this constructor setup the client's attributes,
 * it sets status_ to CONNECTED
 *
 * @param fd client's fd
 * @param server client's parent server
 * @param addr client's address
 */
Client::Client(int fd, Server& server, sockaddr_in& addr): fd_(fd), server_(server), address_(addr), status_(CONNECTED){}

void Client::closeClient()
{
	if (fd_ != -1)
	{
		close(fd_);
		fd_ = -1;
	}
}

Client::~Client()
{
	closeClient();
}

Client::Client(const Client& other): fd_(other.fd_), server_(other.server_), address_(other.address_),
	bufferIn_(other.bufferIn_), bufferOut_(other.bufferOut_), status_(other.status_){}

int Client::getFd() const {return fd_;}

sockaddr_in& Client::getAddress() { return address_;}

clientStatus Client::getStatus () const { return status_;}

Server& Client::getServer() const { return server_;}

void Client::setStatus(clientStatus status) { status_ = status;}

std::string& Client::getBufferIn() { return bufferIn_;}

std::string& Client::getBufferOut() { return bufferOut_;}

void Client::removeFromServer()
{
	server_.removeClient(*this);
}

