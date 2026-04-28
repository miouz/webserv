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

	const std::string&							getPath() const;
	const bool*									getMethods() const;
	const std::string&							getRoot() const;
	const   std::vector<std::string>&			getIndex() const;
	bool										getAutoindex() const;
	const   std::pair<int,std::string>&			getReturn() const;
	const   std::string&						getUploadStore() const;
	const   std::map<std::string,std::string>&	getCgiExtension() const;
	void										print() const;


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
};

	bool		isspace(char c);
	bool		istoken(char c);
	bool		endword(char c);
	std::string	getnextword(std::ifstream &file);

#endif

