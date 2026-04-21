#include "Config.hpp"
#include "Location.hpp"
#include "RequestParser.hpp"

struct responseGetRequest
{
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
		static std::string	response(ServerConfig&, ParsedData&);
	private:
		GetRequest();
		~GetRequest();
		static responseGetRequest	initResponse(Location&, ParsedData&);
		static std::string	generateMessageResponse(const responseGetRequest&);
		static size_t	checkLength(int);
		static void	checkExtension(responseGetRequest&);
		static void	checkSize(std::string&);
		static std::map<std::string, std::string>	mapExtension();
		static std::string generateResponse(ServerConfig&, responseGetRequest&);
		static std::string getMessageCode(int);
		static bool	isGoodPath(responseGetRequest&);
		static bool	isDirectory(std::string&);
		static void	serveFile(responseGetRequest&);
		static void	listDirectory(responseGetRequest&);
		static bool	findIndexPage(Location&, responseGetRequest&);

		static std::string	formatHttpDate(time_t t);
		static std::string	FormatFileDate(time_t t);
};
