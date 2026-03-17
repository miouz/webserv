#include "Server.hpp"

#define SERVER_NUM 1

int main()
{
	Server servers[SERVER_NUM];

	servers[0].setPort(8080);

	for (int i = 0; i < SERVER_NUM; i++)
	{
		try {
		servers[i].setUpServer();
		}
		catch(std::exception& e){
			std::cerr << "cant set up server:" << e.what() << "\n";
			servers[i].closeServer();
		}
	}

	socklen_t len = sizeof(servers[0].getAddress());
	struct sockaddr_in addr = servers[0].getAddress();
	while (1)
	{
		int newFd = accept(servers[0].getSockFd(), (struct sockaddr*)&addr, &len);
		if (newFd > 0)
			std::cout << "Viens la mon mignon\n";
		
	}

	return EXIT_SUCCESS;
}
