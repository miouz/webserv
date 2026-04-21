#include  "webserv.hpp"

volatile sig_atomic_t gStop;

void parentSignalHandler(int signal)
{
	if (signal == SIGINT)
		gStop = true;
}

void initSignals()
{
	struct sigaction sa;
	struct sigaction sa_sigpipe;

	sigemptyset(&sa.sa_mask);
	sigemptyset(&sa_sigpipe.sa_mask);
	sa.sa_flags = 0;
	sa_sigpipe.sa_flags = 0;
	sa.sa_handler = parentSignalHandler;
	sa_sigpipe.sa_handler = SIG_IGN;
	if ( sigaction(SIGINT, &sa, NULL) == RETURN_ERROR)
		throw std::runtime_error(std::string("can't set up signals: ") + strerror(errno));
}

std::vector<ServerConfig> argsToServerConfigs(int ac, char** av)
{
	if (ac != 2)
		throw std::invalid_argument("need 1 config file");
	Config config(av[1]);
	return config.getServers();
}

void	configWebServers(std::vector<ServerConfig>& webserv, Data& data)
{
	data.servers.reserve(1024);
	data.clients.reserve(1024);
	data.fdPool.reserve(1024);
	for (size_t i = 0; i < webserv.size(); i++)
		data.servers.push_back(new Server(webserv[i]));
}

/**
 * @brief this function creat Servers from ServerConfig and setup each server,
 * it adds the correspondant client to clients' pointer vector and pollfd's vector
 *
 * @param data the webserver's data contains servers, clients, pollfd
 * @warning it throws runtime_error
 */
void setUpServers(Data& data)
{
	if (data.servers.size() == 0)
		throw std::runtime_error("no server to run");
	for (size_t i = 0; i < data.servers.size(); i++)
	{
		data.servers[i]->setUpServer();
		pollfd serverFd = fdToPollfdWithStatus(data.servers[i]->getSockFd(), POLLIN);
		data.fdPool.push_back(serverFd);
	}
}
