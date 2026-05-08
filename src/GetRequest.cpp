#include "GetRequest.hpp"
#include <iostream>
#include <string>
#include <sys/dir.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ctime>
#include <fcntl.h>

void	GetRequest::getResponse(responseRequest& responseData, const Location& location)
{
	if (isGoodPath(responseData) == true)
		if (isDirectory(responseData) == true && findIndexPage(location, responseData) == false)
			listDirectory(responseData);
}

bool	GetRequest::findIndexPage(const Location& location, responseRequest& responseData)
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

void	GetRequest::listDirectory(responseRequest& responseData)
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
				+ time + "                  ";
			if (S_ISDIR(sb.st_mode) == true)
				responseData.content += "-\n";
			else
				responseData.content += ss.str() + '\n';
		}
		else
		{
			responseData.content += "</pre><hr></body>\n</html>";
			break ;
		}
	}
	closedir(dirp);
}


bool	GetRequest::isDirectory(responseRequest& responseData)
{
	if (*responseData.path.rbegin() == '/')
		return true;

	struct stat sb;
	if (stat(responseData.path.c_str(), &sb) < 0)
		return false;
	if (S_ISDIR(sb.st_mode) == true)
	{
		responseData.successCode = 301;
		responseData.uri += '/';
	}
	return false;
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


bool	GetRequest::isGoodPath(responseRequest& responseData)
{
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


void	GetRequest::serveFile(responseRequest& responseData)
{
	static const int BUFFER_SIZE = 65536;

	int	fd = open(responseData.path.c_str(), O_RDONLY);
	if (fd < 0)
		responseData.successCode = 403;
	else
	{
		char	buffer[BUFFER_SIZE];
		struct stat sb;

		if (stat(responseData.path.c_str(), &sb) > 0 && sb.st_size > 0)
			responseData.content.reserve(sb.st_size);

		while (1)
		{
			int readData = read(fd, buffer, BUFFER_SIZE);
			if (readData < 0)
			{
				responseData.successCode = 500;
				break ;
			}
			else if (readData == 0)
				break ;
			responseData.content.append(buffer, readData);
		}
		close(fd);
		responseData.contentLength = responseData.content.size();
	}
}
