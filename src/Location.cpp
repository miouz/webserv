#include "Location.hpp"
#include "ServerConfig.hpp"

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
	std::string words[10] = {"", "{", ";", "methods", "root", "index", "autoindex", "return", "upload_store", "cgi_extension"};
	void (Location::*fptr[10])(std::string, std::ifstream&) = {&Location::unexpectedEndException, &Location::unexpectedTokenException, &Location::unexpectedTokenException,
			 &Location::setMethods, &Location::setRoot, &Location::setIndex, &Location::setAutoIndex, &Location::setReturn,
			 	&Location::setUS, &Location::setCGIE};
	
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
		while (i < 10)
		{
			if (word == words[i])
			{
				(this->*fptr[i])(word, file);
				i = 10;
			}
			i++;
		}
		if (i == 10)
			throw std::runtime_error("unknown directive \""+ word + "\"");
		word = getnextword(file);
	}
	this->checkComplete();
}

void	Location::checkComplete()
{
	if (this->root == "")
		throw std::runtime_error("root directive unused");
	if (this->index.size() == 0)
		throw std::runtime_error("index directive unused");
	if (this->_return.first == -1)
		throw std::runtime_error("return directive unused");
	if (this->upload_store == "")
		throw std::runtime_error("upload_store directive unused");
	if (this->cgi_extension.size() == 0)
		throw std::runtime_error("cgi_extension directive unused");
}

void	Location::init()
{
    this->root = "";
    this->autoindex = false;
    this->upload_store = "";
	this->methods[0] = true;
	this->methods[1] = false;
	this->methods[2] = false;
	this->_return.first = -1;
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
	this->_return.first = res;
	this->_return.second = word;
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

 const std::string&							Location::getPath() const
{
	return(this->path);
}

const bool*								Location::getMethods() const
{
	return(this->methods);
}

const std::string&							Location::getRoot() const
{
	return(this->root);
}

const std::vector<std::string>&			Location::getIndex() const
{
	return(this->index);
}

bool								Location::getAutoindex() const
{
	return(this->autoindex);
}

const std::pair<int,std::string>&			Location::getReturn() const
{
	return(this->_return);
}

const std::string&							Location::getUploadStore() const
{
	return(this->upload_store);
}

const std::map<std::string,std::string>&	Location::getCgiExtension() const
{
	return(this->cgi_extension);
}

void	Location::print() const
{
	long unsigned int									i;
	std::map<std::string, std::string>::const_iterator	itstr;


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
	itstr = cgi_extension.begin();
	std::cout << "cgi extensions " << " : " << "\n";
	while (itstr != cgi_extension.end())
	{
   		std::cout << itstr->first << " " << itstr->second << "\n";
		itstr++;
	}
}
