#include "src/webserv.hpp"

int main(int argc, char** argv)
{
	Data data;

	try {
		std::vector<ServerConfig> webserv = argsToServerConfigs(argc, argv);
		configWebServers(webserv, data);
		setUpServers(data);
	} catch (std::exception& e) {
		std::cerr<< "Error Fatal: " << e.what() << "\n";
		shutDownServers(data.servers);
		exit(EXIT_FAILURE);
	}
	pollEventsLoop(data);
	shutDownServers(data.servers);
	return EXIT_SUCCESS;
}
