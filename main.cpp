#include "src/webserv.hpp"

int main()
{
	//TODO: argv error check
	//TODO: creat config class with config file(try-catch)

	Data data;

	data.servers.reserve(1024);
	data.clients.reserve(1024);
	data.fdPool.reserve(1024);


	//set up each server
	setUpServers(data);
	pollEventsLoop(data);
	shutDownServers(data.servers);
	return EXIT_SUCCESS;
}
