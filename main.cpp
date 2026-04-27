#include "src/webserv.hpp"

int main(int argc, char** argv)
{
	Data data;

	try {
		initSignals();
		std::vector<ServerConfig> webserv = argsToServerConfigs(argc, argv);
		configWebServers(webserv, data);
		setUpServers(data);
	} catch (std::exception& e) {
		std::cerr<< "Error Fatal: " << e.what() << "\n";
		shutDownServers(data.servers);
		exit(EXIT_FAILURE);
	}
	pollEventsLoop(data);
	return EXIT_SUCCESS;
}
