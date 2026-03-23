#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>

#define RETURN_ERROR -1

class Server;

enum clientStatus
{
	CONNECTED,
	REQUEST_COMPLETE,
	RESPONSE_READY,
	DISCONNECT
};
