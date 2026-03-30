#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <iostream>
# include <vector>
# include "Location.hpp"
# include <map>
# include <string>

class ServerConfig
{

public : 

	ServerConfig(std::ifstream &file);
	ServerConfig(const ServerConfig &copy);
	ServerConfig &operator=(const ServerConfig &copy);
	~ServerConfig( void );

	std::vector<Location>		getLocations();
	std::string					getListen();
	std::string					getServerName();
	std::string					getRoot();
	std::vector<std::string>	getIndex();
	std::map<int, std::string>	getErrorPage();
	int							getClientMaxBodySize();

	void						print();

private :

	ServerConfig( void );

	void	unexpectedEndException(std::string word, std::ifstream &file);
	void	unexpectedVariableEndException(std::string word, std::ifstream &file);
	void	unexpectedTokenException(std::string word, std::ifstream &file);
	void	setLocation(std::string word, std::ifstream &file);
	void	setName(std::string w, std::ifstream &file);
	void	setRoot(std::string w, std::ifstream &file);
	void	setIndex(std::string w, std::ifstream &file);
	void	setErrorPage(std::string w, std::ifstream &file);
	void	setCMBS(std::string w, std::ifstream &file);
	void	setListen(std::string w, std::ifstream &file);
	void	init();

	std::vector<Location>		locations;
	std::string					listen;
	std::string					server_name;
	std::string					root;
	std::vector<std::string>	index;
	std::map<int, std::string>	error_page;
	int							client_max_body_size;

};

bool	isReadableNumber(std::string word);
void	checkIfWord(std::string word, std::ifstream &file);



#endif