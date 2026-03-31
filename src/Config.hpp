#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <vector>
# include "ServerConfig.hpp"

class Config
{

public : 

	Config(const std::string toread);
	~Config( void );
	std::vector<ServerConfig>	getServers();
	void	print();

private : 

	Config( void );
	Config(Config &copy);
	Config &operator=(Config const &copy);

	std::vector<ServerConfig> servers;
};

#endif
