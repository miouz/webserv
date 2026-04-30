#ifndef POSTREQUEST_HPP
# define POSTREQUEST_HPP

#include "Location.hpp"
#include "RequestParser.hpp"
#include "Request.hpp"

class PostRequest
{
	public:
		static void	postResponse(const ServerConfig&, responseRequest&, const ParsedData&);
	private:
		PostRequest();
		~PostRequest();
		static responseRequest	initResponse(Location&, const ParsedData&);
		static std::string generateResponse(const ServerConfig&, responseRequest&);

		static bool	isUpload(const std::string&);
		static bool	checkData(const ParsedData&, const ServerConfig&, responseRequest&);
		static bool	isMethodAllowed(const std::string&, const Location&);
		static void	createFile(responseRequest&, const std::string&);
};

#endif
