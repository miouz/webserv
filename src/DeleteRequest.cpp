#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int    deleteResponse(std::string file, std::string uri)
{
    const char *str = file.c_str();

    if (uri == "/")
        return (403);
    if (access(str, F_OK) != 0)
        return (200);
    if (access(str, X_OK) != 0)
		return (403);
	if (remove(str) == 0)
		return (403);
	return (200);
}
