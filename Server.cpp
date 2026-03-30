#include "Server.hpp"
#include "Config.hpp"
#include <iostream>
#include <fstream>

	Server::Server( void )
{
}

	Server::Server(const Server &copy)
{
    this->locations = copy.locations;
    this->listen = copy.listen;
    this->server_name = copy.server_name;
    this->root = copy.root;
    this->index = copy.index;
    this->error_page = copy.error_page;
    this->client_max_body_size = copy.client_max_body_size;
}

Server	&Server::operator=(const Server &copy)
{
    if (this == &copy)
        return (*this);
    this->locations = copy.locations;
    this->listen = copy.listen;
    this->server_name = copy.server_name;
    this->root = copy.root;
    this->index = copy.index;
    this->error_page = copy.error_page;
    this->client_max_body_size = copy.client_max_body_size;
	return (*this);
}

	Server::~Server( void )
{
}

	Server::Server(std::ifstream &file)
{
	std::string	word;
	std::string words[10] = {"", "{", ";", "location", "server_name", "root", "index", "error_page", "client_max_body_size", "listen"};
	void (Server::*fptr[10])(std::string word, std::ifstream &file) = {&Server::unexpectedEndException, &Server::unexpectedTokenException, &Server::unexpectedTokenException,
			 &Server::setLocation, &Server::setName, &Server::setRoot, &Server::setIndex, &Server::setErrorPage, &Server::setCMBS, &Server::setListen};

	this->init();
	word = getnextword(file);
	if (word != "{")
		throw std::runtime_error("directive \"server\" has no opening \"{\"");
	word = getnextword(file);
	while (word != "}")
	{
		int i = 0;
		while (i < 10)
		{
			if (word == words[i])
			{
				(this->*fptr[i])(word, file);
				i = 10;
			}
			i++;
		}
		if (i == 10)
			throw std::runtime_error("unknown directive \""+ word + "\"");
		word = getnextword(file);
	}
}

void	Server::init()
{
    this->listen = "";
    this->server_name = "";
    this->root = "";
    this->client_max_body_size = -1;
}

void	Server::unexpectedEndException(std::string word, std::ifstream &file)
{
	(void) word;
	(void) file;
	throw std::runtime_error("unexpected end of file, expecting \"}\"");
}

void	Server::unexpectedVariableEndException(std::string word, std::ifstream &file)
{
	(void) word;
	(void) file;
	throw std::runtime_error("unexpected end of directive, expecting \";\"");
}

void	Server::unexpectedTokenException(std::string word, std::ifstream &file)
{
	(void) file;
	throw std::runtime_error("unexpected \"" + word + "\"");
}

void	Server::setLocation(std::string word, std::ifstream &file)
{
		Location	temp(file);

		(void) word;
		this->locations.push_back(temp);
}

void	Server::setName(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	this->server_name = word;
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

void	Server::setRoot(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	this->root = word;
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

void	Server::setIndex(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	while (word != ";")
	{
		if (word == "")
			this->unexpectedVariableEndException(word, file);
		this->index.push_back(word);
		word = getnextword(file);
	}
}

void	Server::setErrorPage(std::string w, std::ifstream &file)
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

void	Server::setCMBS(std::string w, std::ifstream &file)
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

void	Server::setListen(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	this->listen = word;
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}


std::vector<Location>		Server::getLocations()
{
	return (this->locations);
}
std::string					Server::getListen()
{
	return (this->listen);
}
std::string					Server::getServerName()
{
	return (this->server_name);
}
std::string					Server::getRoot()
{
	return (this->root);
}
std::vector<std::string>	Server::getIndex()
{
	return (this->index);
}
std::map<int, std::string>	Server::getErrorPage()
{
	return (this->error_page);
}
int							Server::getClientMaxBodySize()
{
	return (this->client_max_body_size);
}

void	Server::print()
{
	long unsigned							i;
	std::map<int, std::string>::iterator	it;

	std::cout << "server_name : " << server_name << "\n";
	std::cout << "listen : " << listen << "\n";
	std::cout << "root : " << root << "\n";
	i = 0;
	while (i < index.size())
	{
		std::cout << "index " << i + 1 << " : " << index[i] << "\n";
		i++;
	}
	it = error_page.begin();
	std::cout << "error pages " << " : " << "\n";
	while (it != error_page.end())
	{
   		std::cout << it->first << " " << it->second << "\n";
		it++;
	}
	std::cout << "client_max_body_size : " << client_max_body_size << "\n";
	i = 0;
	while (i < locations.size())
	{
		std::cout << "Location ";
		locations[i].print();
		i++;
		std::cout << "\n";
	}
}