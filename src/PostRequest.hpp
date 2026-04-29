#include "Config.hpp"
#include "Location.hpp"
#include "RequestParser.hpp"

enum METHODS
{
	GET,
	POST,
	DELETE
};

struct responsePostRequest
{
	std::string	root;
	std::string	protocol;
	std::string	uri;
	int	successCode;
	std::string	server;
	std::string	path;
	std::string	contentType;
	std::string	content;
	size_t	contentLength;
};

class PostRequest
{
	public:
		static std::string	response(const ServerConfig&, const ParsedData&);
	private:
		PostRequest();
		~PostRequest();
		static responsePostRequest	initResponse(Location&, const ParsedData&);
		static std::string generateResponse(const ServerConfig&, responsePostRequest&);

		static bool	isUpload(const std::string&);
		static bool	checkData(const std::string&, const std::string&);
		static bool	isMethodAllowed(const std::string&, const Location&);
		static void	createFile(const std::string&, const std::string&);
};
