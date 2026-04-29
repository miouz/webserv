#ifndef GETREQUEST_HPP
# define GETREQUEST_HPP

#include "Location.hpp"
#include "Request.hpp"

class GetRequest
{
	public:
		static void	getResponse(responseRequest&, Location&);
		static void	serveFile(responseRequest&);
	private:
		GetRequest();
		~GetRequest();

	//get
		static bool	isGoodPath(responseRequest&);
		static bool	isDirectory(responseRequest&);
		static void	listDirectory(responseRequest&);
		static bool	findIndexPage(Location&, responseRequest&);
	
	//time
		static std::string FormatFileDate(time_t);
};

#endif
