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
	bool		isCgi;
	int	successCode;
	std::string	server;
	std::string	path;
	std::string	contentType;
	std::string	content;
	std::string	cookie;
	std::string	returnLocation;
	bool	autoIndex;
	bool	listDirectory;
	size_t	contentLength;
};

//TODO: for better optimization, preload mapExtension
class Request
{
	public:
		static int    deleteResponse(const std::string&, const std::string&);
		static bool	isCgi(const std::string&, const Location&);
		static std::string	response(const ServerConfig&, ParsedData&, const Location&);
	private:
		Request();
		~Request();

		static bool	isMethodAllowed(int, const Location&);
		static responseRequest	initResponse(const ServerConfig&, const Location&, ParsedData&);
		static std::string generateResponse(const ServerConfig&, responseRequest&);
		static std::string getMessageCode(int);
		static bool	isRedirect(const responseRequest&);
		static bool	isEscaping(responseRequest&);
		static void	serveFile(responseRequest&);
	//
		static void	parseCgi(ParsedData&, responseRequest&);
	//
	//time
		static std::string	formatHttpDate(time_t);

		static void	checkExtension(responseRequest&, std::map<std::string, std::string>);
};

#endif
