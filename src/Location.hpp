#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <vector>
# include <fstream>
# include <map>
# include <cstdlib>

class Location
{

public : 

	Location(std::ifstream &file);
	Location(const Location &copy);
	~Location( void );
	Location &operator=(Location const &copy);

	std::string							getPath();
	bool*								getMethods();
	std::string							getRoot();
	std::vector<std::string>			getIndex();
	bool								getAutoindex();
	std::pair<int,std::string>			getReturn();
	std::string							getUploadStore();
	std::map<std::string,std::string>	getCgiExtension();
	std::map<int, std::string>			getErrorPage();
	std::string							getCgiPass();
	int									getClientMaxBodySize();
	void								print();


private :

	Location( void );
	void	unexpectedEndException(std::string word, std::ifstream &file);
	void	unexpectedVariableEndException(std::string word, std::ifstream &file);
	void	unexpectedTokenException(std::string word, std::ifstream &file);
	void	setMethods(std::string w, std::ifstream &file);
	void	setRoot(std::string w, std::ifstream &file);
	void	setIndex(std::string w, std::ifstream &file);
	void	setAutoIndex(std::string w, std::ifstream &file);
	void	setReturn(std::string w, std::ifstream &file);
	void	setUS(std::string w, std::ifstream &file);
	void	setCGIE(std::string w, std::ifstream &file);
	void	setCGIP(std::string w, std::ifstream &file);
	void	setCMBS(std::string w, std::ifstream &file);
	void	init();
	void	checkComplete();
	
	std::string							path;
	bool								methods[3];
	std::string							root;
	std::vector<std::string>			index;
	bool								autoindex;
	std::pair<int,std::string>			_return;
	std::string							upload_store;
	std::map<std::string,std::string>	cgi_extension;
	std::map<int, std::string>			error_page;
	std::string							cgi_pass;
	int									client_max_body_size;
};

	bool		isspace(char c);
	bool		istoken(char c);
	bool		endword(char c);
	std::string	getnextword(std::ifstream &file);

#endif

