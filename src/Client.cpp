#include "Client.hpp"
#include "Server.hpp"

/**
 * @brief this constructor setup the client's attributes,
 * it sets status_ to CONNECTED
 *
 * @param fd client's fd
 * @param server client's parent server
 * @param addr client's address
 * @param creationTime client's creationTime by server
 */
Client::Client(int fd, Server& server, sockaddr_in& addr, time_t& creationTime): fd_(fd), cgiPid_(0),
	server_(server), address_(addr), sent_(0), written_(0), status_(CONNECTED), lastActivityTime_(creationTime)
{ cgiFd_[READ] = -1; cgiFd_[WRITE] = -1;}

void Client::closeCgiWriteFd()
{
	if (cgiFd_[WRITE] != -1)
	{
		close(cgiFd_[WRITE]);
		cgiFd_[WRITE] = -1;
	}
}

void Client::closeCgiReadFd()
{
	if (cgiFd_[READ] != -1)
	{
		close(cgiFd_[READ]);
		cgiFd_[READ] = -1;
	}
}

void Client::killCgi()
{
	if (cgiPid_ != 0)
	{
		kill(cgiPid_, SIGINT);
		cgiPid_ = 0;
	}
}

void Client::closeClient()
{
	if (fd_ != -1)
	{
		close(fd_);
		fd_ = -1;
	}
	closeCgiReadFd();
	closeCgiWriteFd();
	killCgi();
}

Client::~Client()
{
	closeClient();
}

int Client::getFd() const {return fd_;}

int* Client::getCgiFd() { return cgiFd_;}

pid_t Client::getCgiPid() const {return cgiPid_;}

void Client::setCgiPid(pid_t pid) { cgiPid_ = pid;}

sockaddr_in& Client::getAddress() { return address_;}

clientStatus Client::getStatus () const { return status_;}

Server& Client::getServer() const { return server_;}

void Client::setStatus(clientStatus status) { status_ = status;}

std::string& Client::getBufferOut() { return bufferOut_;}

time_t	Client::getLastActivityTime() const { return lastActivityTime_; }

RequestParser& Client::getRequest() { return request_; }

void Client::removeFromServer()
{
	server_.removeClient(this);
}

/**
 * @brief this function check if the client hits a timeout(CLIENT_TIMEOUT_SECONDES) sinc lastActivityTime_
 *
 * @return true or false
 */
bool Client::isTimeOut()
{
	time_t now = std::time(NULL);
	if (now == -1)
	{
		std::cerr<< "cant check client's time out, disconnecting client\n";
		return true;
	}
	double elapsed = std::difftime(now, lastActivityTime_);
	if (elapsed >= CLIENT_TIMEOUT_SECONDES)
		return true;
	return false;
}

/**
 * @brief this function update the Client's lastActivityTime_ to now
 */
void Client::updateLastActivityTime()
{
	time_t now = std::time(NULL);
	if (now == -1)
	{
		std::cerr<< "cant check client's time out, disconnecting client\n";
		return;
	}
	lastActivityTime_ = now;
}

/**
 * @brief [TODO: build request response]
 */
void Client::buildResponse()
{
	if (request_.isCgi())
	{
		Cgi cgi(this->server_, this->request_);
		int i = cgi.execute(this->cgiFd, this->cgiPid);
		close(pipeout[0]);
		waitpid(pid, &exitStatus, 0);

	if (WIFEXITED(exitStatus) && WEXITSTATUS(exitStatus) == 0)
		return (200);
		server errors with i 

	}
	else 

/**
 * @brief write body to pipeIn write side for cgi's stdin 
 *
 * @return fully written true or false
 */
bool Client::cgiWriteBody()
{
	std::string body = parsedRequest_.getData().body;
	if (written_ < body.size())
	{
		ssize_t written = write(cgiFd_[WRITE], body.c_str() + written_, body.size() - written_);
		if (written >= 0)
		{
			written_ += written;
			if (written_ < body.size())
			return false;
		}
	}
	return true;
}

/**
 * @brief [TODO:send request response]
 */
bool Client::sendResponse()
{
	if (sent_ < bufferOut_.size())
	{
		ssize_t sent = send(fd_, bufferOut_.c_str() + sent_, bufferOut_.size() - sent_, 0);
		if (sent >= 0)
		{
			sent_ += sent;
			if (sent_ < bufferOut_.size())
			return false;
		}
	}
	return true;
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
	out << "Client on fd " << client.getFd()
		<<", port " << client.getAddress().sin_port << ":";
	switch(client.getStatus())
	{
		case CONNECTED:
			out << "CONNECTED";
			break;
		case REQUEST_COMPLETE:
			out << "REQUEST_COMPLETE";
			break;
		case RESPONSE_READY:
			out << "RESPONSE_READY";
			break;
		case DISCONNECT:
			out << "DISCONNECT";
	}
	time_t lastActivityTime = client.getLastActivityTime();
	out << " , last activity time at " << std::ctime(&lastActivityTime) << " \n";
	
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
void	disconnectClient(int fd, std::vector<Client*>& clients,
					  std::vector<pollfd>& fdPool, std::vector<pollfd>::iterator toRemove)
{
	if (toRemove != fdPool.end())
		fdPool.erase(toRemove);
	Client* client = findClient(fd, clients);
	if (client)
	{
		std::vector<Client*>::iterator clientFound = std::find(clients.begin(),
														 clients.end(), client);
		clients.erase(clientFound);
	#ifdef DEBUG
	std::cout << "\nClient on fd " << client->getFd() << " is DISCONNECTED now\n";
	#endif
	client->removeFromServer();
	}
}
