#include "RequestParser.hpp"
#include <cstdlib>
#include <iostream>
#include <errno.h>

RequestParser::RequestParser(): buffer_(""), isStartParsed_(false), isHeadersParsed_(false), isBodyParsed_(false)
{
	data_.code = 200;
	data_.isCgi = false;
}

RequestParser::~RequestParser() {}

void	RequestParser::feed(const std::string& chunk)
{
	buffer_ += chunk;

	if (isStartParsed_ == false)
		parseStartLine();
	if (isStartParsed_ == true && isHeadersParsed_ == false)
		parseHeaders();
	if (isHeadersParsed_ == true && isBodyParsed_ == false)
		parseBody();
}

void	RequestParser::parseStartLine()
{
	std::string	EOL("\r\n");

	size_t	found = buffer_.find(EOL);
	if (found == std::string::npos)
		return ;
	std::istringstream	iss(buffer_.substr(0, found));
	buffer_ = buffer_.substr(found + EOL.size());
	data_.method = getToken(iss);
	data_.uri = getToken(iss);
	data_.protocol = getToken(iss);
	checkStartLine();
	isStartParsed_ = true;

	#ifdef DEBUG
		std::cout << "method = [" << data_.method << "]\n";
		std::cout << "uri = [" << data_.uri << "]\n";
		std::cout << "protocol = [" << data_.protocol << "]\n";
	#endif
}

void	RequestParser::parseHeaders()
{
	std::string	sep = ":";
	std::string	key;
	std::string	value;

	#ifdef DEBUG
		std::cout << "\n[key:value]\n";
	#endif
	while (buffer_.find("\r\n") != std::string::npos && isComplete() == false)
	{
		std::string	line = getLine(buffer_);
		if (line.empty())
		{
			isHeadersParsed_ = true;
			return ;
		}
		std::size_t	found = line.find(sep);
		if (found == std::string::npos)
			continue ;
		key = line.substr(0, found);
		value = line.substr(found + sep.size());
		setHeader(key, value);
	}
}

void	RequestParser::parseBody()
{
	std::map<std::string, std::string>::iterator contentLen = data_.headers.find("CONTENT-LENGTH");
	if (contentLen != data_.headers.end())
	{
		char	*endptr;
		errno = 0;
		
		long	totalSize = std::strtol(contentLen->second.c_str(), &endptr, 10);
		if (errno == ERANGE || (endptr && *endptr))
		{
			endParsing(400);
			return ;
		}
		if (static_cast<long>(buffer_.size()) < totalSize)
			return ;
		data_.body = buffer_.substr(0, totalSize);
	}
	#ifdef DEBUG
		std::cout << "\nBody = {\n" << data_.body << "\n}\n";
	#endif
	isBodyParsed_ = true;
}

std::string	RequestParser::getLine(std::string& str) const
{
	std::string	EOL = "\r\n";
	std::size_t	found = str.find(EOL);
	std::string	line;

	if (found != std::string::npos)
	{
		line = str.substr(0, found);
		str = str.substr(found + EOL.size());
	}
	return line;
}

std::string	RequestParser::getToken(std::istringstream& iss) const
{
	std::string	token;
	iss >> token;
	return token;
}

void	RequestParser::setHeader(std::string& key, std::string& value)
{
	std::string	token;
	std::istringstream	iss(value);
	
	capitalize(key);
	if (key == "HOST" || key == "CONTENT-LENGTH")
	{
		iss >> token;
		if (iss >> token)
		{
			std::cout << "WTFFFFFF\n";
			endParsing(400);
			return ;
		}
	}

	#ifdef DEBUG
		std::cout << "\n[" << key << ":" << value << "]\n";
	#endif

	checkHeaders(key, value);
	if (isComplete() == false)
		data_.headers[key] = value;
}

void	RequestParser::checkStartLine()
{
	static const int	NBR_METHODS = 3;

	if (data_.uri[0] != '/')
	{
		endParsing(400);
		return ;
	}
	if (data_.protocol.empty())
	{
		if (data_.method != "GET")
		{
			endParsing(400);
			return ;
		}
		isHeadersParsed_ = true;
	}
	if (data_.protocol == "HTTP/1.0" || data_.protocol == "HTTP/1.1")
	{
		std::string	methods[NBR_METHODS] = {"GET", "POST", "DELETE"};
		for (int i = 0; i < NBR_METHODS; i++)
			if (data_.method == methods[i])
				return ;
	}
	endParsing(400);
}

void	RequestParser::checkHeaders(std::string& key, std::string& value)
{
	if (key.find(" ") != std::string::npos)
		endParsing(400);
	else if ((key == "HOST" || key == "CONTENT-LENGTH") && value.empty())
		endParsing(400);
	else if (key == "HOST" && data_.headers.find("HOST") != data_.headers.end())
		endParsing(400);
	else if (key == "CONTENT-LENGTH" && data_.headers.find("CONTENT-LENGTH") != data_.headers.end())
		endParsing(400);
}

void	RequestParser::endParsing(int code)
{
	isStartParsed_ = true;
	isHeadersParsed_ = true;
	isBodyParsed_ = true;
	data_.code = code;
}

bool	RequestParser::isComplete() const
{
	return isBodyParsed_;
}

ParsedData&	RequestParser::getData()
{
	return data_;
}

void	capitalize(std::string& str)
{
	for (size_t i = 0; i < str.size(); i++)
		str[i] = toupper(str[i]);
}
