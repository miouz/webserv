#include "PostRequest.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cstdlib>

PostRequest::PostRequest() {}

PostRequest::~PostRequest() {}

void	PostRequest::postResponse(const ServerConfig& config, responseRequest& response, const ParsedData& data)
{
	if (isUpload(response.uri) == false)
		response.successCode = 403;
	else if (checkData(data, config, response) == false)
		response.successCode = 403;
	else
		createFile(response, data.body);
}

bool	PostRequest::isUpload(const std::string& uri)
{
	size_t	found = uri.find("/upload/");

	if (found != std::string::npos && found == 0)
		return true;
	return false;
}

bool	PostRequest::checkData(const ParsedData& data, const ServerConfig& config, responseRequest& response)
{
	std::string	prefix = "/upload/";
	std::string	fileName = data.uri.substr(prefix.size());
	if (fileName.find('/') != std::string::npos || fileName.empty())
		return false;

	std::string	file = response.path;
	const std::string	len = data.headers.at("CONTENT-LENGTH");

	if (atoi(len.c_str()) > config.getClientMaxBodySize())
		return false;
	if (access(file.c_str(), F_OK) < 0)
		return true;
	return false;
}

void	PostRequest::createFile(responseRequest& response, const std::string& content)
{
	std::ofstream ofs(response.path.c_str(), std::ios::binary);
	if (ofs.good() != true)
		response.successCode = 403;
	else
		ofs.write(content.data(), content.size());
}

