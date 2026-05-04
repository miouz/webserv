#include "Cgi.hpp"
#include <unistd.h>
#include "ServerConfig.hpp"
#include <sstream>
#include "RequestParser.hpp"
#include "Location.hpp"
#include <sys/wait.h>
#include <cstring>
#include <string>

std::string readAll(int fd);
Location findLocation(std::string& uri, ServerConfig& server);

	Cgi::Cgi( void )
{
}

	Cgi::Cgi(Cgi &copy)
{
	(void) copy;
}

Cgi	&Cgi::operator=(Cgi const &copy)
{
	(void) copy;
	return (*this);
}

	Cgi::~Cgi( void )
{
}

	Cgi::Cgi(ServerConfig server, ParsedData request)
{
	method        = request.method;
	body          = request.body;
	contentLength = request.body.size();

	std::string uri = request.uri;
	size_t qPos = uri.find('?');
	std::string uriWithoutQuery = (qPos != std::string::npos) ? uri.substr(0, qPos) : uri;
	queryString = (qPos != std::string::npos) ? uri.substr(qPos + 1) : "";

	Location loc = findLocation(uri, server);
	std::map<std::string, std::string> cgiMap = loc.getCgiExtension();
	std::map<std::string, std::string>::iterator it;
	for (it = cgiMap.begin(); it != cgiMap.end(); ++it)
	{
		size_t extPos = uriWithoutQuery.find(it->first);
		if (extPos != std::string::npos)
		{
			scriptName = uriWithoutQuery.substr(0, extPos + it->first.size());
			pathInfo   = uriWithoutQuery.substr(extPos + it->first.size());
			interpreterPath = it->second;
			break;
		}
	}	

	scriptPath  = loc.getRoot() + scriptName;
	workingDir  = scriptPath.substr(0, scriptPath.rfind('/'));

	serverPort = server.getListen();

	std::map<std::string,std::string>::const_iterator ctIt = request.headers.find("Content-Type");
	contentType = (ctIt != request.headers.end()) ? ctIt->second : "";
	for (std::map<std::string,std::string>::const_iterator h = request.headers.begin(); h != request.headers.end(); ++h)
	{
		if (h->first == "Content-Type" || h->first == "Content-Length")
			continue;
		std::string key = "HTTP_";
		for (size_t i = 0; i < h->first.size(); i++)
			key += (h->first[i] == '-') ? '_' : toupper(h->first[i]);
		httpHeaders[key] = h->second;
	}
}

int Cgi::execute(Client& client, std::vector<pollfd>& fdPool)
{
	int      pipein[2] = {-1, -1};
	int      pipeout[2] = {-1, -1};
	pid_t    pid;
	char**   env;

	if (access(scriptPath.c_str(), F_OK | X_OK) != 0)
		return (404);

	env = buildEnv();

	if (pipe(pipeout) == RETURN_ERROR)
	{
		freeEnv(env);
		return (500);
	}
	if (method == "POST")
	{
		if (pipe(pipein) == RETURN_ERROR)
		{
			freeEnv(env);
			close(pipeout[READ]);
			close(pipeout[WRITE]);
			return (500);
		}
	}

	pid = fork();

	if (pid < 0)
	{
		freeEnv(env);
		close(pipeout[READ]);
		close(pipeout[WRITE]);
		if (pipein[READ] != -1)
			close(pipein[READ]);
		if (pipein[WRITE] != -1)
			close(pipein[WRITE]);
		return (500);
	}

	if (pid > 0)
	{
		if (pipein[READ] != -1)
			close(pipein[READ]);
		close(pipeout[WRITE]);
		if (method == "POST")
		{
			client.getCgiFd()[WRITE] = pipein[WRITE];
			fcntl(pipein[WRITE], F_SETFL, O_NONBLOCK);
		}
		client.getCgiFd()[READ] = pipeout[READ];
		fcntl(pipeout[READ], F_SETFL, O_NONBLOCK);
		client.setCgiPid(pid);
		client.addCgiFdsToPool(fdPool);
		freeEnv(env);
		return (0);
	}

	if (pid == 0)
		execChild(env, pipein, pipeout);
	return (500);
}

void	Cgi::execChild(char** env, int* pipein, int*pipeout)
{
	close(pipeout[READ]);
	if (pipein[WRITE] != -1)
		close(pipein[WRITE]);
	if (pipein[READ] != -1)
	{
		dup2(pipein[READ], STDIN_FILENO);
		close(pipein[READ]);
	}
	dup2(pipeout[WRITE], STDOUT_FILENO);
	close(pipeout[WRITE]);
	chdir(workingDir.c_str());
	char* argv[3];
	argv[0] = const_cast<char*>(interpreterPath.c_str());
	argv[1] = const_cast<char*>(scriptPath.c_str());
	argv[2] = NULL;
	execve(argv[0], argv, env);
	freeEnv(env);
	exit(1);
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
	std::ostringstream ossPort;
	ossPort << serverPort;
	envVec.push_back("SERVER_PORT=" + ossPort.str());
	envVec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	if (method == "POST")
	{
		std::ostringstream oss;
		oss << contentLength;
		envVec.push_back("CONTENT_LENGTH=" + oss.str());
	}
	std::map<std::string, std::string>::iterator it;
	for (it = httpHeaders.begin(); it != httpHeaders.end(); ++it)
		envVec.push_back(it->first + "=" + it->second);

	char** env = new char*[envVec.size() + 1];
	for (size_t i = 0; i < envVec.size(); i++)
	{
		env[i] = new char[envVec[i].size() + 1];
		std::strcpy(env[i], envVec[i].c_str());
	}
	env[envVec.size()] = NULL;
	return env;
}

Location findLocation(std::string& uri, ServerConfig& server)
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

void Cgi::freeEnv(char** env)
{
	if (!env)
		return ;
	for (int i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
}
