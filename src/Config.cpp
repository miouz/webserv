#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

	Config::Config( void )
{
}

	Config::Config(Config &copy)
{
	(void) copy;
}

	Config::Config(const std::string toread)
{
	std::ifstream						file(toread.c_str());
	std::string 						word;
	std::map<std::string, std::string>	mapExtension;

	if (file.is_open() == false)
		throw std::runtime_error("Error opening file: " + toread);
	word = getnextword(file);
	mapExtension = this->mapExtension();
	while (word != "")
	{
		if (word != "server")
		{
			if (istoken(word[0]) == true)
				throw std::runtime_error("unexpected \"" + word + "\"");
			throw std::runtime_error("unknown directive \""+ word + "\"");
		}
		ServerConfig	temp(file, mapExtension);
		servers.push_back(temp);
		word = getnextword(file);
	}
	if (servers.size() == 0)
			throw std::runtime_error("directive \"server\" missing");

	file.close();
}

std::map<std::string, std::string> Config::mapExtension()
{
	std::ifstream	ifs("mime.types");
	std::map<std::string, std::string> 	map;
	std::string	token;
	std::string	type;

	if (ifs.is_open() == false)
		throw std::runtime_error("Error opening file: mime.types");
	while (ifs >> token)
	{
		if (token.find("/") != std::string::npos)
			type = token;
		else
			map[token] = type;
	}
	return map;
}


	Config::~Config( void )
{
}

Config	&Config::operator=(Config const &copy)
{
	(void) copy;
	return (*this);
}

std::string	getnextword(std::ifstream &file)
{
	static std::string	read = "";
	std::string			res = "";
	int					i;
	int					j;

	i = 0;
	while(isspace(read[i]) == true)
		i++;
	while(read[i] == '\0')
	{
		if (!getline(file, read))
			return ("");
		i = 0;
		while(isspace(read[i]) == true)
			i++;
	}
	j = 1;
	if (istoken(read[i]) == false)
	{
		while (endword(read[i + j]) == false)
			j++;
	}
	res = read.substr(i, j);
	i = i + j;
	j = 0;
	while (read[i + j] != '\0' && read[i + j] != '\n')
		j++;
	read = read.substr(i, j);
	return (res);
}

bool	isspace(char c)
{
	if (c == ' ' || c == '\t' || c == '\n')
		return (true);
	return (false);
}

bool	istoken(char c)
{
	if (c == '{' || c == '}' || c == ';')
		return (true);
	return (false);
}

bool	endword(char c)
{
	if (isspace(c) || istoken(c) || c == 0)
		return (true);
	return (false);
}

void	Config::print()
{
	long unsigned int	i;

	i = 0;
	while (i < servers.size())
	{
		std::cout << "Server #" << i + 1 << " : \n";
		servers[i].print();
		i++;
		std::cout << "\n";
	}
}

std::vector<ServerConfig>	Config::getServers()
{
	return (this->servers);
}