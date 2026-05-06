#ifndef REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

#include <map>
#include <string>
#include <sstream>

struct ParsedData
{
	std::string	method;
	std::string	uri;
	std::string	queryString;
	bool		isCgi;
	std::string	protocol;
	std::map<std::string, std::string>	headers;
	std::string	body;
	int	code;
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

		
		void	removeQueryString(std::string&, std::string&);
		void	setHeader(std::string&, std::string&);
		void	checkHeaders(std::string&, std::string&);
		void	checkStartLine();
		void	endParsing(int);


	public:
		RequestParser();
		~RequestParser();
		void	feed(const std::string&);
		bool	isComplete() const;
		ParsedData&	getData();
};

void	capitalize(std::string&);

#endif
