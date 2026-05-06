#ifndef CGI_HPP
# define CGI_HPP
# include <string>
# include <map>
# include <vector>
# include "RequestParser.hpp"
# include "Client.hpp"

// Forward declarations — full definitions are only needed in Cgi.cpp
class ServerConfig;

class Cgi
{
public:
    ~Cgi( void );
    Cgi(ServerConfig server, ParsedData request);
    int execute(Client&, std::vector<pollfd>&);
private:
    Cgi( void );
    Cgi(Cgi &copy);
    Cgi     &operator=(Cgi const &copy);
    void    execChild(char** env, int* pipein, int* pipeout);
    void    freeEnv(char** env);
    char**  buildEnv();

    std::string                         scriptPath;
    std::string                         ext;
    std::string                         interpreterPath;
    std::string                         workingDir;
    std::string                         method;
    std::string                         queryString;
    std::string                         body;
    std::string                         contentType;
    size_t                              contentLength;
    int                                 serverPort;
    std::string                         scriptName;
    std::string                         pathInfo;
    std::map<std::string, std::string>  httpHeaders;
};
#endif
