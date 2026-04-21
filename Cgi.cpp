#include "Cgi.hpp"
#include <unistd.h>
#include "src/ServerConfig.hpp"
#include <ostringstream>

	Cgi::Cgi( void )
{
}

	Cgi::Cgi(Cgi &copy)
{
}

Cgi	&Cgi::operator=(Cgi const &copy)
{
}

	Cgi::~Cgi( void )
{
}

	Cgi::Cgi(ServerConfig server, ParsedData request)
{
	size_t		pos;
	Location	loc;
	std::string extension;

	method = request.method;
	body = request.body;
	contentLength = request.body.size();
	serverPort = server.getListen();
	pos = request.uri.substr('?');
	scriptName = request.uri.substr(0, pos);
	queryString = request.uri.substr(pos + 1);
	pos = scriptName.rfind('.');
	while(scriptName[pos] && scriptName[pos] != '/')
		pos++;
	pathInfo = scriptName.substr(pos);
	scriptName = scriptName.substr(0, pos - 1)
	loc = findLocation(request.uri, server);
	scriptPath = loc.getRoot() + scriptName;
	workingDir = scriptPath.substr(0, scriptPath.rfind('/'));
	contentType = request.headers["Content-Type"];
	extension = scriptName.substr(scriptName.rfind('.'));
	interpreterPath = loc.getCgiExtension()[extension];
	pos = request.uri.substr(request.uri.find(extension) + extension.size());
	this->httpHeaders=request.headers;
}

int	Cgi::execute(Response& response);
{
	int							pipein[2];
	int							pipeout[2];
	int							pid;
	std::vector<char*>			env;

	if (access(scriptPath.c_str(), F_OK | X_OK) != 0)
	    return (404);
	env = buildEnv();
	if (pipe(pipein) < 0 || pipe(pipeout) < 0)
		return 500;
	pid = fork()
	if (pid < 0)
		return (500);
	if (pid == 0)
		execChild(env, pipein, pipeout);
	close(pipein[0]);
	close(pipeout[1]);
	if (method == "POST" && !body.empty())
		write(stdin_pipe[1], body.c_str(), body.size());
	close(stdin_pipe[1]);
	std::string cgiOutput;
	char buffer[4096];
	ssize_t bytes;
	while ((bytes = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0)
		cgiOutput.append(buffer, bytes);
	close(stdout_pipe[0]);
	int status;
	waitpid(pid, &status, 0);
}

void	Cgi::execChild(std::vector<char*> env, int* pipein, int*pipeout)
{
	dup2(stdin_pipe[0],  0);
	dup2(stdout_pipe[1], 1);
	close(stdin_pipe[1]);
	close(stdout_pipe[0]);
	close(1);
	chdir(workingDir.c_str());
	char* argv[3];
	argv[0] = const_cast<char*>(interpreterPath.c_str());
	argv[1] = const_cast<char*>(scriptPath.c_str());
	argv[2] = NULL;
	execve(argv[0], argv, env.data());
	exit(1);	exit(1);
}

char** Cgi::buildEnv()
{
	std::vector<std::string> envVec;

	envVec.push_back("REQUEST_METHOD="  + method);
	envVec.push_back("QUERY_STRING="    + queryString);
	envVec.push_back("CONTENT_TYPE="    + contentType);
	envVec.push_back("SCRIPT_FILENAME=" + scriptPath);
	envVec.push_back("SCRIPT_NAME="     + scriptName);
	envVec.push_back("PATH_INFO="       + pathInfo);
	envVec.push_back("SERVER_NAME="     + serverName);
	envVec.push_back("SERVER_PORT="     + serverPort);
	envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	if (method == "POST")
	{
		std::ostringstream oss;
		oss << contentLength;
		envVec.push_back("CONTENT_LENGTH=" + oss.str());
	}

	std::map<std::string, std::string>::iterator it;
	for (it = ctx.httpHeaders.begin(); it != ctx.httpHeaders.end(); ++it)
    	envVec.push_back(it->first + "=" + it->second);
	std::vector<char*> env;
	for (size_t i = 0; i < envVec.size(); i++)
		env.push_back(const_cast<char*>(envVec[i].c_str()));
	env.push_back(NULL);
	return(env);
}

Location findLocation(const std::string& uri, const ServerConfig& server)
{
    std::vector<Location> locations = server.getLocations();
    Location bestMatch;
    size_t bestLen;

	bestLen = 0;
    for (size_t i = 0; i < locations.size(); i++)
    {
        std::string locPath = locations[i].getPath();
        if (uri.find(locPath) == 0)
        {
            if (locPath.size() > bestLen)
            {
                bestLen   = locPath.size();
                bestMatch = locations[i];
            }
        }
    }
    return bestMatch;
}