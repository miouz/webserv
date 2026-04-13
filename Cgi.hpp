#ifndef CGI_HPP
# define CGI_HPP

class Cgi
{

public : 

	Cgi( void );
	Cgi(Cgi &copy);
	Cgi &operator=(Cgi const &copy);
	~Cgi( void );

	void	launch() const;

};

#endif