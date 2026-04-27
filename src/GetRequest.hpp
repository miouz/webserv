#include "Config.hpp"
#include "Location.hpp"
#include "RequestParser.hpp"

struct responseGetRequest
{
	std::string	root;
	std::string	protocol;
	std::string	uri;
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
class GetRequest
{
	public:
		static std::string	response(const ServerConfig&, const ParsedData&);
	private:
		GetRequest();
		~GetRequest();
		static responseGetRequest	initResponse(Location&, const ParsedData&);
		static std::string	generateMessageResponse(responseGetRequest&);
		static size_t	checkLength(int);
		static void	checkExtension(responseGetRequest&);
		static void	checkSize(std::string&);
		static std::map<std::string, std::string>	mapExtension();
		static std::string generateResponse(const ServerConfig&, responseGetRequest&);
		static std::string getMessageCode(int);
		static bool	isGoodPath(responseGetRequest&);
		static bool	isDirectory(responseGetRequest&);
		static bool	isRedirect(const responseGetRequest&);
		static void	serveFile(responseGetRequest&);
		static void	listDirectory(responseGetRequest&);
		static bool	findIndexPage(Location&, responseGetRequest&);

		static std::string	formatHttpDate(time_t t);
		static std::string	FormatFileDate(time_t t);
};
