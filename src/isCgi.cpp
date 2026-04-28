#include "Config.hpp"
#include "RequestParser.hpp"

bool	isCgi(std::string& uri, Location& location)
{
	size_t	found = uri.find_last_of('.');
	std::string	extension = uri.substr(found);
	std::map<std::string, std::string> mapExtension = location.getCgiExtension();
	std::string	 checkExtension = mapExtension[extension];
	if (checkExtension.empty() == true)
		return false;
	return true;
}
