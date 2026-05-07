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

std::string	Request::response(const ServerConfig& config, ParsedData& data, const Location& location)
{
	responseRequest	responseData = initResponse(location, data);
	static const int	NBR_METHODS = 3;
	int	method;
	if (responseData.successCode == 200 && data.isCgi == false)
	{
		checkExtension(responseData, config.getMapExtension());
		if (isCgi(responseData.uri, location) == false)
		{
			std::string	methods[NBR_METHODS] = {"GET", "POST", "DELETE"};
			for (method = GET; method < NBR_METHODS; method++)
				if (methods[method] == data.method)
					break ;
			if (isMethodAllowed(method, location) == false)
				responseData.successCode = 403;
			else
			{
				#ifdef DEBUG
				std::cerr << "Method:" << data.method << " allowed\n";
				#endif
				switch (method)
				{
					case GET:
						GetRequest::getResponse(responseData, location);
						break ;
					case POST:
						PostRequest::postResponse(config, responseData, data, location);
						break ;
					case DELETE:
						responseData.successCode = deleteResponse(responseData.path, responseData.uri);
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

responseRequest	Request::initResponse(const Location& location, ParsedData& data)
{
	responseRequest	 response;

	#ifdef DEBUG
std::cerr << "iscgi :" << data.isCgi << '\n';
#endif
	response.root = location.getRoot() + "/";
	response.protocol = "HTTP/1.0";
	response.server = "webserv";
	response.successCode = data.code;
	if (data.isCgi == true)
		parseCgi(data, response);
	else
		response.contentType = "application/octet-stream";
	response.contentLength = response.content.size();
	response.listDirectory = false;
	response.autoIndex = location.getAutoindex();
	response.uri = data.uri;
	response.isCgi = data.isCgi;
	response.path = location.getRoot() + data.uri;
	return response;
}

std::string	Request::generateResponse(const ServerConfig& config, responseRequest& responseData)
{
	if (responseData.successCode != 200 && responseData.successCode != 201)
	{
		std::map<int, std::string>		errorMap = config.getErrorPage();
		std::string	errorPage = errorMap[responseData.successCode];
		responseData.contentType = "text/html";
		responseData.path = "./" + errorPage;
		responseData.isCgi = false;
	}
	if (responseData.listDirectory == false && responseData.isCgi == false)
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
		+ "Date: " + time + EOL;
	if (responseData.successCode != 201)
	{
		response += "Content-type: " + responseData.contentType + EOL;
		if (responseData.listDirectory == false)
			response += "Content-length: " + ssContentLen.str() + EOL;
	}
	if (isRedirect(responseData))
		response += "Location: " + responseData.uri + "/" + EOL;
	if (responseData.cookie.empty() == false)
		response += "Set-Cookie: " + responseData.cookie + EOL;
	response += "Connection: close" + EOL + EOL;
	if (responseData.successCode != 201)
		response += responseData.content;
#ifdef DEBUG
	std::cerr << "response:\n" << response << '\n';
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

bool	Request::isCgi(const std::string& uri, const Location& location)
{
	if (location.getPath() != "/cgi-bin")
		return false;
	size_t	found = uri.find_last_of(".");
	if (found == std::string::npos)
		return false;
	std::string	extension = uri.substr(found + 1);
	std::map<std::string, std::string> mapExtension = location.getCgiExtension();
	std::string	 checkExtension = mapExtension[extension];
	if (checkExtension.empty() == true)
		return false;
	return true;
}

void	Request::checkExtension(responseRequest& response, std::map<std::string, std::string>	map)
{
	size_t	found = response.uri.find_last_of(".");
	if (found != std::string::npos)
	{
		std::string	extension = response.uri.substr(found + 1);
		std::string	type = map[extension];
		if (type.empty() == false)
			response.contentType = type;
	}
}

void	Request::parseCgi(ParsedData& data, responseRequest& response)
{
	std::string	EOL = "\r\n";
	std::string	EOLEOL = "\r\n\r\n";
	std::string	sep = ":";
	size_t	found;
	#ifdef DEBUG
	std::cerr << "Start Parse cgi\n";
	#endif
	found	= data.body.find(EOLEOL);
	response.contentType = "text/html";
	if (found == std::string::npos)
	{
		#ifdef DEBUG
		std::cerr << "EOL not found\n";
		#endif
		EOL = "\n";
		EOLEOL = "\n\n";
		found	= data.body.find(EOLEOL);
		if (found == std::string::npos)
			return ;
		#ifdef DEBUG
		std::cerr << "EOL  found\n";
		#endif
	}
	do
	{
		found	= data.body.find(EOL);
		std::string	line = data.body.substr(0, found);
	#ifdef DEBUG
	std::cerr << "line ="  <<  line << "\n";
	#endif

		data.body = data.body.substr(line.size() + 1);
		if (line.empty())
		{

		#ifdef DEBUG
		std::cerr << "line ="  <<  line << "\n";
		#endif
			break ;
		}
		size_t	sepFound = line.find(sep);
		if (sepFound == std::string::npos)
		{
			response.successCode = 500;
			return ;
		}
		std::string	key = line.substr(0, sepFound);
		std::string	value = line.substr(sepFound + sep.size(), line.size() - 1);
		capitalize(key);
#ifdef DEBUG
		std::cerr << key << ":" << value << "\n";
#endif
		if (key == "CONTENT-TYPE")
			response.contentType = value;
		if (key == "SET-COOKIE")
			response.cookie = value;

	}
	while (data.body.find(EOLEOL) != std::string::npos);
	response.content = data.body;
}

int    Request::deleteResponse(const std::string& file, const std::string& uri)
{
    const char *str = file.c_str();

	#ifdef DEBUG
	std::cerr << "delete: " << file << "\ncheck uri = [" << uri << "]\n";
	#endif
    if (uri == "/")
        return (403);
    if (access(str, F_OK) != 0)
        return (200);
    if (access(str, W_OK) != 0)
		return (403);
	if (remove(str) == 0)
		return (200);
	return (403);
}
