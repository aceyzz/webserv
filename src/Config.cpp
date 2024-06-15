#include "Config.hpp"

Config::~Config()
{
	for (size_t i = 0; i < _routes.size(); i++)
	{
		if (_routes[i])
			delete _routes[i];
	}
}

Route*	Config::getRoute(std::string uri)
{
	for (size_t i = 0; i < _routes.size(); i++)
	{
		if (_routes[i]->getUri() == uri)
			return _routes[i];
	}
	return nullptr;
}

std::string	Config::getErrorPage(int code)
{
	std::map<int, std::string>::iterator it = _errorPages.find(code);
	if (it != _errorPages.end())
		return (it->second);
	return "";
}

void	Config::printConfig()
{
	std::cout << CYAN "CONFIG: " RST << std::endl;
	std::cout << GOLD "Name: " RST << _name << std::endl;
	std::cout << GOLD "Port: " RST << _port << std::endl;
	std::cout << GOLD "Root: " RST << _root << std::endl;
	std::cout << GOLD "Index: " RST << _index << std::endl;
	std::cout << GOLD "Error pages: " RST << std::endl;
	for (std::map<int, std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << GOLD "Autoindex: " RST << _autoindex << std::endl;
	std::cout << GOLD "Routes: " RST << std::endl;
	for (size_t i = 0; i < _routes.size(); i++)
		_routes[i]->printRoute();
	std::cout << GOLD "Max body size: " RST << _maxBodySize << std::endl;
}
