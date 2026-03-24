#ifndef REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

#include <map>
#include <string>
#include <sstream>

struct ParsedData
{
	std::string	method;
	std::string	uri;
	std::string	protocol;
	std::map<std::string, std::string>	headers;
	std::string	body;
};

class RequestParser
{
	private:
		ParsedData	data_;
		std::string	buffer_;
		bool	isStartParsed_;
		bool	isHeadersParsed_;
		bool	isBodyParsed_;

		void	parseStartLine();
		void	parseHeaders();
		void	parseBody();

		std::string	getLine(std::string&) const;
		std::string	getToken(std::istringstream&) const;

		void	setHeader(std::string&, std::string&);
		void	checkHeaders(std::string&, std::string&);
		void	checkStartLine();


	public:
		RequestParser();
		~RequestParser();
		void	feed(std::string&);
		bool	isComplete() const;
		ParsedData	getData() const;
};

void	capitalize(std::string&);

#endif
