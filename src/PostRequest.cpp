#include "PostRequest.hpp"
#include "fcntl.h"
#include <sys/stat.h>
#include <unistd.h>

PostRequest::PostRequest() {}

PostRequest::~PostRequest() {}

bool	isUpload(const std::string& uri)
{
	size_t	found = uri.find("/upload/");

	if (found != std::string::npos && found == 0)
		return true;
	return false;
}

bool	checkData(const std::string& uri, const std::string& root)
{
	std::string	prefix = "/upload";
	std::string	fileName = uri.substr(prefix.size());
	if (fileName.find('/') != std::string::npos)
		return false;

	std::string	file = root + uri;
	if (access(file.c_str(), F_OK) < 0)
		return true;
	return false;
}
