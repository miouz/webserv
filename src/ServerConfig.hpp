#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

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

	const std::vector<Location>&		getLocations() const;
	int							getListen();
	const std::map<int, std::string>&	getErrorPage() const;
	int							getClientMaxBodySize() const;
	std::string    resolvePath(const std::string uri) const;
	Location findLocation(std::string&) const;

	void						print();

private :

	ServerConfig( void );

	void	unexpectedEndException(std::string word, std::ifstream &file);
	void	unexpectedVariableEndException(std::string word, std::ifstream &file);
	void	unexpectedTokenException(std::string word, std::ifstream &file);
	void	setLocation(std::string word, std::ifstream &file);
	void	setErrorPage(std::string w, std::ifstream &file);
	void	setListen(std::string w, std::ifstream &file);
	void	setCMBS(std::string w, std::ifstream &file);
	void	init();
	void	checkComplete();

	std::vector<Location>		locations;
	int							listen;
	std::map<int, std::string>	error_page;
	int							client_max_body_size;
};

bool	isReadableNumber(std::string word);
void	checkIfWord(std::string word, std::ifstream &file);



#endif
