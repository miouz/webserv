#include "GetRequest.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/dir.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

GetRequest::GetRequest() {}

GetRequest::~GetRequest() {}

std::string	GetRequest::response(const ServerConfig& config, const ParsedData& data)
{
	Location location = config.getLocations()[0];
	responseGetRequest	responseData = initResponse(location, data);

	std::cout <<  "TRYING TO GET " << responseData.uri << '\n';
	if (isGoodPath(responseData) == true)
	{
		if (isDirectory(data.uri) == false || findIndexPage(location, responseData))
			serveFile(responseData);
		else
			listDirectory(responseData);
	}
	return generateResponse(config, responseData);
}

responseGetRequest	GetRequest::initResponse(Location& location, const ParsedData& data)
{
	responseGetRequest	 response;
	
	response.root = location.getRoot() + "/";
	std::cout << "ROOT=[" << response.root << "]\n";
	response.protocol = "HTTP/1.0";
	response.server = "webserv";
	response.successCode = 200;
	response.contentType = "application/octet-stream";
	response.contentLength = 0;
	response.listDirectory = false;
	response.autoIndex = location.getAutoindex();
	response.uri = data.uri;
	response.path = location.getRoot();
	std::cout << response.uri << ":" << response.path << '\n';
	if (data.uri != response.root)
		response.path += data.uri;
	return response;
}

void	GetRequest::checkExtension(responseGetRequest& response)
{
	std::map<std::string, std::string>	map = mapExtension();
	size_t	found = response.uri.find_last_of(".");
	if (found != std::string::npos)
	{
		std::string	extension = response.uri.substr(found + 1);
		std::string	type = map[extension];
		if (type.empty() == false)
			response.contentType = type;
	}
}

std::map<std::string, std::string> GetRequest::mapExtension()
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

std::string	GetRequest::generateResponse(const ServerConfig& config, responseGetRequest& responseData)
{
	if (responseData.successCode != 200)
	{
		std::cout << "SERVE ERROR\n";
		std::map<int, std::string>		errorMap = config.getErrorPage();
		std::string	errorPage = errorMap[responseData.successCode];
		responseData.path = "./" + errorPage;
		std::cout << "Serve: " << responseData.path << '\n';
		serveFile(responseData);
	}
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
	response += "Connection: close" + EOL + EOL
		+ responseData.content;
	return response;
}

bool	GetRequest::isGoodPath(responseGetRequest& responseData)
{
	std::cout << "find :" << responseData.root << " in " << responseData.uri << '\n';
	if (responseData.root == responseData.uri)
	{
		std::cout << "ITIS\n";
		responseData.successCode = 403;
		return false;
	}
	if (access(responseData.path.c_str(), F_OK) < 0)
	{
		responseData.successCode = 404;
		return false;
	}
	if (access(responseData.path.c_str(), R_OK) < 0)
	{
		responseData.successCode = 403;
		return false;
	}
	return true;
}

bool	GetRequest::isDirectory(const std::string& uri)
{
	if (uri.find_last_of("/") + 1 == uri.size())
		return true;
	return false;
}

void	GetRequest::serveFile(responseGetRequest& responseData)
{
	int	fd = open(responseData.path.c_str(), O_RDONLY);
	if (fd < 0)
		responseData.successCode = 403;
	else
	{
		char	buffer[1024];

		checkExtension(responseData);
		while (1)
		{
			int readData = read(fd, buffer, 1024);
			if (readData < 0)
			{
				responseData.successCode = 500;
				break ;
			}
			else if (readData == 0)
				break ;
			responseData.content.append(buffer, readData);
			responseData.contentLength += readData;
		}
	}
	close(fd);
}

void	GetRequest::listDirectory(responseGetRequest& responseData)
{
	DIR* dirp = opendir(responseData.path.c_str());
	dirent* dp;
	struct stat sb;

	responseData.listDirectory = true;
	responseData.contentType = "text/html";
	if (!dirp)
	{
		responseData.successCode = 403;
		return ;
	}
	responseData.content = "<html>\n<head><title>Index of " + responseData.uri
		+ "</title></head>\n<body>\n<h1>Index of "
		+ responseData.uri + "</h1><hr><pre><a href=\"" + "../\">../</a>\n";
	while (dirp)
	{
		if ((dp = readdir(dirp)) != NULL)
		{
			if (std::string(dp->d_name) == ".." || std::string(dp->d_name) == ".")
				continue ;
			std::string	path = responseData.path + dp->d_name;
			stat(path.c_str(), &sb);
			std::string time = FormatFileDate(sb.st_mtim.tv_sec);
			std::stringstream	ss;
			ss << sb.st_size;
			responseData.content += "<a href=\"" + std::string(dp->d_name)
				+ "\">" + std::string(dp->d_name) + "</a>\t\t"
				+ time + "                  "
				+ ss.str() + '\n';
		}
		else
		{
			responseData.content += "</pre><hr></body>\n</html>";
			break ;
		}
	}
	closedir(dirp);
}

bool	GetRequest::findIndexPage(Location& location, responseGetRequest& responseData)
{
	if (responseData.autoIndex == false)
		return false;
	for (size_t i = 0; i < location.getIndex().size(); i++)
	{
		std::string	path = responseData.path + location.getIndex()[i];
		if (access(path.c_str(), F_OK | R_OK) == 0)
		{
			responseData.path = path;
			return true;
		}
	}
	return false;
}

std::string	GetRequest::getMessageCode(int code)
{
	if (code == 200)
		return "OK";
	if (code == 400)
		return "Bad Request";
	if (code == 403)
		return "Forbidden";
	if (code == 404)
		return "Not Found";
	return "UNKOWN";
}

std::string GetRequest::formatHttpDate(time_t t)
{
    char buffer[100];
    std::tm *gmt = std::gmtime(&t);

    std::strftime(buffer, sizeof(buffer),
                  "%a, %d %b %Y %H:%M:%S GMT", gmt);

    return buffer;
}

std::string GetRequest::FormatFileDate(time_t t)
{
    char buffer[100];
    std::tm *local = std::localtime(&t);

    std::strftime(buffer, sizeof(buffer),
                  "%d-%b-%Y %H:%M",
                  local);

    return buffer;
}
