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

Client& Client::operator=(const Client& other)
{
	if (this == &other)
		return *this;
	fd_ = other.fd_;
	server_ = other.server_;
	address_ = other.address_;
	status_ = other.status_;
	return *this;
}

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
	for (size_t i = 0; i < clients.size(); i++)
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

/**
 * @brief this function disconnect a client and clean its resources:
 * firstly it removes its pollfd from the fdPool for poll() to watch,
 * then erases it's pointer from vector clients,
 * lastly it finds the client with the fd and remove the client
 * from the server who owns it(client's destructor called here);
 *
 * @param fd the fd of the client to disconnect
 * @param clients the vector<Client*>'s reference
 * @param fdPool the vector<fdpool>'s reference'
 * @param toRemove the iterator of the fd on fdPool to erase
 * @warning use this function ONLY in a error-handling situation,
 * otherwise the client is always cleaned by its Server owner
 */
void	disconnectClient(int fd, std::vector<Client*> clients,
					  std::vector<pollfd>& fdPool, std::vector<pollfd>::iterator toRemove)
{
	fdPool.erase(toRemove);
	Client* client = findClient(fd, clients);
	if (client)
	{
		std::vector<Client*>::iterator clientFound = std::find(clients.begin(),
														 clients.end(), client);
		clients.erase(clientFound);
	}
	client->removeFromServer();
}
