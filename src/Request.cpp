#include "Request.hpp"
#include "GetRequest.hpp"
#include "PostRequest.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>
#include <iostream>


Request::Request() {}

Request::~Request() {}

std::string	Request::response(const ServerConfig& config, const ParsedData& data, const Location& location)
{
	responseRequest	responseData = initResponse(location, data);
	static const int	NBR_METHODS = 3;
	int	method;
	if (responseData.successCode == 200)
	{
		checkExtension(responseData);
		if (isCgi(responseData.extension, location) == false)
		{
			std::string	methods[NBR_METHODS] = {"GET", "POST", "DELETE"};
			for (method = GET; method < NBR_METHODS; method++)
				if (methods[method] == data.method)
					break ;
			if (isMethodAllowed(method, location) == false)
				responseData.successCode = 403;
			else
			{
				switch (method)
				{
					case GET:
						GetRequest::getResponse(responseData, location);
						break ;
					case POST:
						PostRequest::postResponse(config, responseData, data);
						break ;
					case DELETE:
						break ;
				}
			}
		}
	}
	return generateResponse(config, responseData);
}

bool	Request::isMethodAllowed(int method, const Location& location)
{
	const bool*	methods = location.getMethods();

	if (methods[method] == false)
		return false;
	return true;
}

responseRequest	Request::initResponse(const Location& location, const ParsedData& data)
{
	responseRequest	 response;

	response.root = location.getRoot() + "/";
	response.protocol = "HTTP/1.0";
	response.server = "webserv";
	response.successCode = data.code;
	response.contentType = "application/octet-stream";
	response.contentLength = 0;
	response.listDirectory = false;
	response.autoIndex = location.getAutoindex();
	response.uri = data.uri;
	response.path = location.getRoot() + data.uri;
	return response;
}

std::string	Request::generateResponse(const ServerConfig& config, responseRequest& responseData)
{
	if (responseData.successCode != 200)
	{
		std::map<int, std::string>		errorMap = config.getErrorPage();
		std::string	errorPage = errorMap[responseData.successCode];
		responseData.contentType = "text/html";
		responseData.path = "./" + errorPage;
	}
	if (responseData.listDirectory == false)
		GetRequest::serveFile(responseData);
	time_t	t = time(NULL);
	std::string time = formatHttpDate(t);
	std::string		EOL("\r\n");
	std::stringstream	ssSuccessCode;
	std::stringstream	ssContentLen;
	ssSuccessCode << responseData.successCode;
	ssContentLen << responseData.contentLength;

	std::string	response = responseData.protocol
		+ " " + ssSuccessCode.str()
		+ " " + getMessageCode(responseData.successCode) + EOL
		+ "Server: " + responseData.server + EOL
		+ "Date: " + time + EOL
		+ "Content-type: " + responseData.contentType + EOL;
	if (responseData.listDirectory == false)
		response += "Content-length: " + ssContentLen.str() + EOL;
	if (isRedirect(responseData))
		response += "Location: " + responseData.uri + "/" + EOL;
	response += "Connection: close" + EOL + EOL
		+ responseData.content;
#ifdef DEBUG
	std::cout << "response:\n" << response << '\n';
#endif
	return response;
}

bool	Request::isRedirect(const responseRequest& responseData)
{
	if (responseData.successCode == 301)
		return true;
	return false;
}

std::string	Request::getMessageCode(int code)
{
	switch (code)
	{
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 301:
			return "Moved Permanently";
		case 400:
			return "Bad Request";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 500:
			return "Internal Server Error";
		default:
			return "UNKNOWN";
	}
}

std::string Request::formatHttpDate(time_t t)
{
	char buffer[100];
	std::tm *gmt = std::gmtime(&t);

	std::strftime(buffer, sizeof(buffer),
			   "%a, %d %b %Y %H:%M:%S GMT", gmt);

	return buffer;
}

std::map<std::string, std::string> Request::mapExtension()
{
	std::ifstream	ifs("mime.types");
	std::map<std::string, std::string> 	map;
	std::string	token;
	std::string	type;

	while (ifs >> token)
	{
		if (token.find("/") != std::string::npos)
			type = token;
		else
			map[token] = type;
	}
	return map;
}

bool	Request::isCgi(const std::string& extension, const Location& location)
{
	std::map<std::string, std::string> mapExtension = location.getCgiExtension();
	std::string	 checkExtension = mapExtension[extension];
	if (checkExtension.empty() == true)
		return false;
	return true;
}

void	Request::checkExtension(responseRequest& response)
{
	std::map<std::string, std::string>	map = mapExtension();
	size_t	found = response.uri.find_last_of(".");
	if (found != std::string::npos)
	{
		std::string	extension = response.uri.substr(found + 1);
		std::string	type = map[extension];
		if (type.empty() == false)
		{
			response.contentType = type;
			response.extension = extension;
		}
	}
}
