#include "Cgi.hpp"
#include <unistd.h>

	Cgi::Cgi( void )
{
}

	Cgi::Cgi(Cgi &copy)
{
}

Cgi	&Cgi::operator=(Cgi const &copy)
{
}

	Cgi::~Cgi( void )
{
}

void	Cgi::launch() const
{
	int		pipetab[2];
	int		pid;

	if (pipe(pipetab) == -1)
		throw ketchose;
	pid = fork()
	if (pid == -1)
		throw ketchosedaut;
	if (pid == 0)
	{



	}

	
	

}