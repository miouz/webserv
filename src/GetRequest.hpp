#include "Config.hpp"
#include "RequestParser.hpp"

struct responseGetRequest
{
	std::string	protocol;
	int	successCode;
	std::string	server;
	std::string	contentType;
	std::string	extension;
	size_t	contentLength;
};

class GetRequest
{
	public:
		static std::string	response(ServerConfig&, ParsedData&);
	private:
		GetRequest();
		~GetRequest();
		static responseGetRequest	initResponse();
		static std::string	generateMessageResponse(const responseGetRequest&);
		static size_t	checkLength(int);
		static void	checkExtension(std::string&, responseGetRequest&);
		static std::map<std::string, std::string>	mapExtension();
};
