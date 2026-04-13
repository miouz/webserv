#include "ServerConfig.hpp"
#include "Config.hpp"
#include <iostream>
#include <fstream>

	ServerConfig::ServerConfig( void )
{
}

	ServerConfig::ServerConfig(const ServerConfig &copy)
{
    this->locations = copy.locations;
    this->listen = copy.listen;
    this->error_page = copy.error_page;
}

ServerConfig	&ServerConfig::operator=(const ServerConfig &copy)
{
    if (this == &copy)
        return (*this);
    this->locations = copy.locations;
    this->listen = copy.listen;
    this->error_page = copy.error_page;
	return (*this);
}

	ServerConfig::~ServerConfig( void )
{
}

	ServerConfig::ServerConfig(std::ifstream &file)
{
	std::string	word;
	std::string words[7] = {"", "{", ";", "location", "error_page", "listen", "client_max_body_size"};
	void (ServerConfig::*fptr[7])(std::string word, std::ifstream &file) = {&ServerConfig::unexpectedEndException, &ServerConfig::unexpectedTokenException, &ServerConfig::unexpectedTokenException,
			 &ServerConfig::setLocation, &ServerConfig::setErrorPage, &ServerConfig::setListen, &ServerConfig::setCMBS};

	this->init();
	word = getnextword(file);
	if (word != "{")
		throw std::runtime_error("directive \"server\" has no opening \"{\"");
	word = getnextword(file);
	while (word != "}")
	{
		int i = 0;
		while (i < 7)
		{
			if (word == words[i])
			{
				(this->*fptr[i])(word, file);
				i = 7;
			}
			i++;
		}
		if (i == 7)
			throw std::runtime_error("unknown directive \""+ word + "\"");
		word = getnextword(file);
	}
	this->checkComplete();
}

void	ServerConfig::checkComplete()
{
	if (this->listen == -1)
		throw std::runtime_error("listen directive unused");
	if (this->client_max_body_size == -1)
		throw std::runtime_error("client_max_body_size directive unused");
	if (this->locations.size() == 0)
		throw std::runtime_error("location directive unused");
	if (this->error_page.size() == 0)
		throw std::runtime_error("error_page directive unused");
}

void	ServerConfig::init()
{
    this->listen = -1;
	this->client_max_body_size = -1;
}

void	ServerConfig::unexpectedEndException(std::string word, std::ifstream &file)
{
	(void) word;
	(void) file;
	throw std::runtime_error("unexpected end of file, expecting \"}\"");
}

void	ServerConfig::unexpectedVariableEndException(std::string word, std::ifstream &file)
{
	(void) word;
	(void) file;
	throw std::runtime_error("unexpected end of directive, expecting \";\"");
}

void	ServerConfig::unexpectedTokenException(std::string word, std::ifstream &file)
{
	(void) file;
	throw std::runtime_error("unexpected \"" + word + "\"");
}

void	ServerConfig::setLocation(std::string word, std::ifstream &file)
{
		Location	temp(file);

		(void) word;
		this->locations.push_back(temp);
}

void	ServerConfig::setErrorPage(std::string w, std::ifstream &file)
{
	std::string	word;
	int			res;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	if (isReadableNumber(word) == false)
		throw std::runtime_error("\"error_page\" directive invalid value");
	res = atoi(word.c_str());
	word = getnextword(file);
	checkIfWord(word, file);
	this->error_page[res] = word;
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

void	ServerConfig::setCMBS(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	if (isReadableNumber(word) == false)
		throw std::runtime_error("\"client_max_body_size\" directive invalid value");
	this->client_max_body_size = atoi(word.c_str());
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

bool	isReadableNumber(std::string word)
{
	int	i;

	i = 0;
	while (word[i])
	{
		if (word[i] < '0' || word[i] > '9')
			return (false);
		i++;
	}
	return (true);
}

void	checkIfWord(std::string word, std::ifstream &file)
{
	(void) file;
	if (word == "")
		throw std::runtime_error("unexpected end of file, expecting \"}\"");
	if (istoken(word[0]) == true)
		throw std::runtime_error("unexpected \"" + word + "\"");
}

void	ServerConfig::setListen(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	if (isReadableNumber(word) == false)
		throw std::runtime_error("\"listen\" directive invalid value");
	this->listen = atoi(word.c_str());
	if (this->listen > 65535)
		throw std::runtime_error("\"listen\" directive invalid value");
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}


std::vector<Location>		ServerConfig::getLocations()
{
	return (this->locations);
}
int							ServerConfig::getListen()
{
	return (this->listen);
}
int							ServerConfig::getClientMaxBodySize()
{
	return (this->client_max_body_size);
}
std::map<int, std::string>	ServerConfig::getErrorPage()
{
	return (this->error_page);
}

void	ServerConfig::print()
{
	long unsigned							i;
	std::map<int, std::string>::iterator	it;

	std::cout << "listen : " << listen << "\n";
	it = error_page.begin();
	std::cout << "client max body size : " << client_max_body_size << "\n";
	std::cout << "error pages " << " : " << "\n";
	while (it != error_page.end())
	{
   		std::cout << it->first << " " << it->second << "\n";
		it++;
	}
	i = 0;
	while (i < locations.size())
	{
		std::cout << "Location ";
		locations[i].print();
		i++;
		std::cout << "\n";
	}
}