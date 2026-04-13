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
    this->root = copy.root;
    this->index = copy.index;
    this->error_page = copy.error_page;
}

ServerConfig	&ServerConfig::operator=(const ServerConfig &copy)
{
    if (this == &copy)
        return (*this);
    this->locations = copy.locations;
    this->listen = copy.listen;
    this->root = copy.root;
    this->index = copy.index;
    this->error_page = copy.error_page;
	return (*this);
}

	ServerConfig::~ServerConfig( void )
{
}

	ServerConfig::ServerConfig(std::ifstream &file)
{
	std::string	word;
	std::string words[8] = {"", "{", ";", "location", "root", "index", "error_page", "listen"};
	void (ServerConfig::*fptr[8])(std::string word, std::ifstream &file) = {&ServerConfig::unexpectedEndException, &ServerConfig::unexpectedTokenException, &ServerConfig::unexpectedTokenException,
			 &ServerConfig::setLocation, &ServerConfig::setRoot, &ServerConfig::setIndex, &ServerConfig::setErrorPage, &ServerConfig::setListen};

	this->init();
	word = getnextword(file);
	if (word != "{")
		throw std::runtime_error("directive \"server\" has no opening \"{\"");
	word = getnextword(file);
	while (word != "}")
	{
		int i = 0;
		while (i < 8)
		{
			if (word == words[i])
			{
				(this->*fptr[i])(word, file);
				i = 8;
			}
			i++;
		}
		if (i == 8)
			throw std::runtime_error("unknown directive \""+ word + "\"");
		word = getnextword(file);
	}
	this->checkComplete();
}

void	ServerConfig::checkComplete()
{
	if (this->listen == -1)
		throw std::runtime_error("listen directive unused");
	if (this->root == "")
		throw std::runtime_error("root directive unused");
	if (this->index.size() == 0)
		throw std::runtime_error("index directive unused");
	if (this->locations.size() == 0)
		throw std::runtime_error("location directive unused");
	if (this->error_page.size() == 0)
		throw std::runtime_error("error_page directive unused");
}

void	ServerConfig::init()
{
    this->listen = -1;
    this->root = "";
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

void	ServerConfig::setRoot(std::string w, std::ifstream &file)
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

void	ServerConfig::setIndex(std::string w, std::ifstream &file)
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
std::string					ServerConfig::getRoot()
{
	return (this->root);
}
std::vector<std::string>	ServerConfig::getIndex()
{
	return (this->index);
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
	i = 0;
	while (i < locations.size())
	{
		std::cout << "Location ";
		locations[i].print();
		i++;
		std::cout << "\n";
	}
}