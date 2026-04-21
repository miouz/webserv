#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>

class Cgi
{

public : 

	~Cgi( void );
	Cgi::Cgi(ServerConfig server, ParsedData request)

	int		Cgi::execute(Response& response);

private :

	Cgi( void );
	Cgi(Cgi &copy);
	Cgi &operator=(Cgi const &copy);

	void	Cgi::execChild(std::vector<char*> env, int* pipein, int*pipeout);


	char** Cgi::buildEnv();

	std::string							scriptPath;
	std::string							interpreterPath;
	std::string							workingDir;
	std::string							method;
	std::string							queryString;
	std::string							body;
	std::string							contentType;
	size_t								contentLength;
	std::string							serverName;
	int			   						serverPort;
	std::string							scriptName;
	std::string							pathInfo;
	std::map<std::string, std::string>	httpHeaders;

};

#endif