#include "RequestParser.hpp"
#include <cstdlib>
#include <iostream>

RequestParser::RequestParser(): buffer_(""), isStartParsed_(false), isHeadersParsed_(false), isBodyParsed_(false) {}

RequestParser::~RequestParser() {}

void	RequestParser::feed(std::string& chunk)
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
	{
		throw std::runtime_error("Out of scope: Time out");//WARNING: tmp: out of scope
		return ;
	}
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

	if (isHeadersParsed_ == false && buffer_.find("\r\n\r\n") != std::string::npos)
		isHeadersParsed_ = true;
	#ifdef DEBUG
		std::cout << "\n[key:value]\n";
	#endif
	while (1)
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
		size_t	totalSize = atoi(contentLen->second.c_str());
		if (buffer_.size() < totalSize)
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
	iss >> value;
	capitalize(key);
	if (key == "HOST" && iss >> token)
		throw std::runtime_error("400 Bad Request");

	#ifdef DEBUG
		std::cout << "\n[" << key << ":" << value << "]\n";
	#endif

	checkHeaders(key, value);
	data_.headers[key] = value;
}

void	RequestParser::checkStartLine()
{
	static const int	NBR_METHODS = 3;
	if (data_.protocol.empty())
	{
		isHeadersParsed_ = true;
		if (data_.method != "GET")
			throw std::runtime_error("400 Bad Request");
	}
	// if (data_.protocol != "HTTP/1.0")//WARNING: We have to discuss what can be accepted
	// 	throw std::runtime_error("400 Bad Request");
	if (data_.uri[0] != '/')
		throw std::runtime_error("400 Bad Request");

	std::string	methods[NBR_METHODS] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < NBR_METHODS; i++)
		if (data_.method == methods[i])
			return ;
	throw std::runtime_error("400 Bad Request");
}

void	RequestParser::checkHeaders(std::string& key, std::string& value)
{
	if (key.find(" ") != std::string::npos)
		throw std::runtime_error("400 Bad Request");
	if (key == "HOST" && value.empty())
		throw std::runtime_error("400 Bad Request");
	if (key == "HOST" && data_.headers.find("HOST") != data_.headers.end())
		throw std::runtime_error("400 Bad Request");

}

bool	RequestParser::isComplete() const
{
	return isBodyParsed_;
}

const ParsedData&	RequestParser::getData() const
{
	return data_;
}

void	capitalize(std::string& str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (std::islower(str[i]))
			str[i] -= 'a' - 'A';
}
