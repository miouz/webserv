#include "RequestParser.hpp"
#include <iostream>
#include <cstdlib>
#include "fcntl.h"
#include "unistd.h"

int main(void)
{
	char buf[1024];
	RequestParser	parser;
	try
	{
		while (parser.isComplete() == false)
		{
			read(0, buf, 1024);
			std::string	str(buf);
			parser.feed(str);
		}
		ParsedData	data = parser.getData();
		std::cout << "method = " << data.method << '\n';
		std::cout << "uri = " << data.uri << '\n';
		std::cout << "headers { " << '\n';
		for (std::map<std::string, std::string>::iterator it = data.headers.begin(); it != data.headers.end(); it++)
			std::cout << it->first << ": " << it->second << '\n';
		std::cout << "}\n";
	}
	catch (std::exception& e)
	{
		std::cout << "Error: " << e.what() << '\n';
	}
}
