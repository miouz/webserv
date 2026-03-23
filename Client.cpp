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

Client* findClient(int fd, std::vector<Client*>& clients)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (fd == clients[i]->getFd())
			return (clients[i]);
	}
	return NULL;
}


std::ostream& operator<<(std::ostream& out, Client& client)
{
	char ip4[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client.getAddress().sin_addr), ip4, INET_ADDRSTRLEN);
	out << "client on fd " << client.getFd()
		<< ", address " << ip4 << ", on port " << client.getAddress().sin_port << "\n";
	return out;
}

