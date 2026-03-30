#include "Location.hpp"
#include "Config.hpp"
#include "Server.hpp"

	Location::Location( void )
{
}

	Location::Location(const Location &copy)
{
{
    this->path = copy.path;
    this->root = copy.root;
    this->autoindex = copy.autoindex;
    this->_return = copy._return;
    this->upload_store = copy.upload_store;
    this->cgi_extension = copy.cgi_extension;
    this->error_page = copy.error_page;
    this->cgi_pass = copy.cgi_pass;
    this->client_max_body_size = copy.client_max_body_size;
    this->index = copy.index;
    for (int i = 0; i < 3; i++)
        this->methods[i] = copy.methods[i];
}
}

Location	&Location::operator=(const Location &copy)
{
    if (this == &copy)
        return (*this);
    this->path = copy.path;
    this->root = copy.root;
    this->autoindex = copy.autoindex;
    this->_return = copy._return;
    this->upload_store = copy.upload_store;
    this->cgi_extension = copy.cgi_extension;
    this->error_page = copy.error_page;
    this->cgi_pass = copy.cgi_pass;
    this->client_max_body_size = copy.client_max_body_size;
    this->index = copy.index;
    for (int i = 0; i < 3; i++)
        this->methods[i] = copy.methods[i];
    return (*this);
}

	Location::~Location( void )
{
}

	Location::Location(std::ifstream &file)
{
	std::string	word;
	std::string words[12] = {"", "{", ";", "methods", "root", "index", "autoindex", "return", "upload_store", "cgi_extension", "cgi_path", "client_max_body_size"};
	void (Location::*fptr[12])(std::string, std::ifstream&) = {&Location::unexpectedEndException, &Location::unexpectedTokenException, &Location::unexpectedTokenException,
			 &Location::setMethods, &Location::setRoot, &Location::setIndex, &Location::setAutoIndex, &Location::setReturn,
			 	&Location::setUS, &Location::setCGIE, &Location::setCGIP, &Location::setCMBS};
	
	this->init();
	word = getnextword(file);
	checkIfWord(word, file);
	this->path = word;
	word = getnextword(file);
	if (word != "{")
		throw std::runtime_error("directive \"location\" has no opening \"{\"");
	word = getnextword(file);
	while (word != "}")
	{
		int i = 0;
		while (i < 13)
		{
			if (word == words[i])
			{
				(this->*fptr[i])(word, file);
				i = 13;
			}
			i++;
		}
		if (i == 13)
			throw std::runtime_error("unknown directive \""+ word + "\"");
		word = getnextword(file);
	}
}

void	Location::init()
{
    this->path = "";
    this->root = "";
    this->autoindex = false;
    this->upload_store = "";
    this->cgi_pass = "";
    this->client_max_body_size = 1;
	this->methods[0] = true;
	this->methods[1] = false;
	this->methods[2] = false;
}

void	Location::unexpectedEndException(std::string word, std::ifstream &file)
{
	(void) word;
	(void) file;
	throw std::runtime_error("unexpected end of file, expecting \"}\"");
}

void	Location::unexpectedVariableEndException(std::string word, std::ifstream &file)
{
	(void) word;
	(void) file;
	throw std::runtime_error("unexpected end of directive, expecting \";\"");
}


void	Location::unexpectedTokenException(std::string word, std::ifstream &file)
{
	(void) file;
	throw std::runtime_error("unexpected \"" + word + "\"");
}

void Location::setMethods(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	this->methods[0] = false;
	while (word != ";")
	{
		if (word == "")
			this->unexpectedVariableEndException(word, file);
		if (word == "GET")
			this->methods[0] = true;
		else if (word == "POST")
			this->methods[1] = true;
		else if (word == "DELETE")
			this->methods[2] = true;
		else
			throw std::runtime_error("\"methods\" directive invalid value");
		word = getnextword(file);
	}
}

void	Location::setRoot(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	this->root = word;
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

void	Location::setIndex(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	while (word != ";")
	{
		if (word == "")
			this->unexpectedVariableEndException(word, file);
		this->index.push_back(word);
		word = getnextword(file);
	}
}

void Location::setAutoIndex(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	if (word == "")
		this->unexpectedVariableEndException(word, file);
	if (word == "on")
		this->autoindex = true;
	else if (word == "off")
		this->autoindex = false;
	else
		throw std::runtime_error("\"auto_index\" directive invalid value");
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

void Location::setReturn(std::string w, std::ifstream &file)
{
	std::string	word;
	int			res;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	if (isReadableNumber(word) == false)
		throw std::runtime_error("\"return\" directive invalid value");
	res = atoi(word.c_str());
	word = getnextword(file);
	checkIfWord(word, file);
	this->error_page.insert(std::make_pair(res, word));
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

void Location::setUS(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	this->upload_store = word;
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

				
void Location::setCGIE(std::string w, std::ifstream &file)
{
	std::string	word;
	std::string	nextword;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	nextword = getnextword(file);
	checkIfWord(nextword, file);
	this->cgi_extension[word] = nextword;
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}
				
void Location::setCGIP(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	this->cgi_pass = word;
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}

void	Location::setCMBS(std::string w, std::ifstream &file)
{
	std::string	word;

	(void) w;
	word = getnextword(file);
	checkIfWord(word, file);
	if (isReadableNumber(word) == false)
		throw std::runtime_error("\"client_max_body_size\" directive invalid value");
	this->client_max_body_size = atoi(word.c_str());
	word = getnextword(file);
	if (word != ";")
		this->unexpectedVariableEndException(word, file);
}


std::string							Location::getPath()
{
	return(this->path);
}

bool*								Location::getMethods()
{
	return(this->methods);
}

std::string							Location::getRoot()
{
	return(this->root);
}

std::vector<std::string>			Location::getIndex()
{
	return(this->index);
}

bool								Location::getAutoindex()
{
	return(this->autoindex);
}

std::pair<int,std::string>			Location::getReturn()
{
	return(this->_return);
}

std::string							Location::getUploadStore()
{
	return(this->upload_store);
}

std::map<std::string,std::string>	Location::getCgiExtension()
{
	return(this->cgi_extension);
}

std::map<int, std::string>			Location::getErrorPage()
{
	return(this->error_page);
}

std::string							Location::getCgiPass()
{
	return(this->cgi_pass);
}

int									Location::getClientMaxBodySize()
{
	return(this->client_max_body_size);
}

void	Location::print()
{
	long unsigned int								i;
	std::map<int, std::string>::iterator			itint;
	std::map<std::string, std::string>::iterator	itstr;


	std::cout << path << "\n";
	std::cout << "GET : " << methods[0] << "; POST : ";
	std::cout << methods[1] << "; DELETE : " << methods[2] << "\n";
	std::cout << "root : " << root << "\n";
	i = 0;
	while (i < index.size())
	{
		std::cout << "index " << i + 1 << " : " << index[i] << "\n";
		i++;
	}
	std::cout << "autoindex : " << autoindex << "\n";
	std::cout << "return : " << _return.first << " " <<  _return.second << "\n";
	std::cout << "upload_store : " << upload_store << "\n";
	itint = error_page.begin();
	std::cout << "error pages " << " : " << "\n";
	while (itint != error_page.end())
	{
   		std::cout << itint->first << " " << itint->second << "\n";
		itint++;
	}
	std::cout << "client_max_body_size : " << client_max_body_size << "\n";
	std::cout << "cgi_pass : " << cgi_pass << "\n";
	itstr = cgi_extension.begin();
	std::cout << "cgi extensions " << " : " << "\n";
	while (itstr != cgi_extension.end())
	{
   		std::cout << itstr->first << " " << itstr->second << "\n";
		itstr++;
	}
}