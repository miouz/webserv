#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "ServerConfig.hpp"
#include "Location.hpp"
#include "RequestParser.hpp"

enum METHODS
{
	GET,
	POST,
	DELETE
};

struct responseRequest
{
	std::string	root;
	std::string	protocol;
	std::string	uri;
	std::string	extension;
	int	successCode;
	std::string	server;
	std::string	path;
	std::string	contentType;
	std::string	content;
	bool	autoIndex;
	bool	listDirectory;
	size_t	contentLength;
};

//TODO: for better optimization, preload mapExtension
class Request
{
	public:
		static bool	isCgi(const std::string&, const Location&);
		static std::string	response(const ServerConfig&, const ParsedData&, const Location&);
	private:
		Request();
		~Request();

		static bool	isMethodAllowed(int, const Location&);
		static responseRequest	initResponse(const Location&, const ParsedData&);
		static std::string generateResponse(const ServerConfig&, responseRequest&);
		static std::string getMessageCode(int);
		static bool	isRedirect(const responseRequest&);
		static bool	isEscaping(responseRequest&);
		static void	serveFile(responseRequest&);
	//
	//time
		static std::string	formatHttpDate(time_t);

		static void	checkExtension(responseRequest&);
		static std::map<std::string, std::string>	mapExtension();
};

#endif
