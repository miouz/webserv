#include "GetRequest.hpp"
#include "exception"
#include <fcntl.h>

GetRequest::GetRequest() {}

GetRequest::~GetRequest() {}

//TODO: mandatory is serve file, optionnal is autoindex if index not present
std::string	GetRequest::response(ServerConfig& config, ParsedData& data)
{
	responseGetRequest	response = initResponse();
	std::string	path = config.getRoot() + data.uri;
	int	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
		throw std::runtime_error("fail openning: " + data.uri);
	checkExtension(data.uri, response);
	//checkSize(path, response);
	

}

responseGetRequest	GetRequest::initResponse()
{
	responseGetRequest	 response;
	
	response.protocol = "HTTP/1.0";
	response.server = "webserv";
	response.successCode = 200;
	response.extension = "application/octet-stream";
	return response;
}

//TODO: for better optimization, preload it in webserv programm
void	GetRequest::checkExtension(std::string& uri, responseGetRequest& response)
{
	std::map<std::string, std::string>	map = mapExtension();
	std::string	type = map[uri];
	if (type.empty() == false)
		response.contentType = type;
}


std::map<std::string, std::string> mapExtension()
{
	std::ifstream	ifs("mime.types");
	std::map<std::string, std::string> 	map;
	std::string	token;
	std::string	type;
	
	while (ifs >> token)
	{
		if (token.find("/"))
			type = token;
		else
			map[token] = type;
	}
	return map;
}
