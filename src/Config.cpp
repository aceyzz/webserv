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

void	Config::setErrorMessages()
{
	_errorMessages[200] = "OK";
	_errorMessages[201] = "Created";
	_errorMessages[204] = "No Content";
	_errorMessages[301] = "Moved Permanently";
	_errorMessages[302] = "Found";
	_errorMessages[303] = "See Other";
	_errorMessages[307] = "Temporary Redirect";
	_errorMessages[400] = "Bad Request";
	_errorMessages[401] = "Unauthorized";
	_errorMessages[403] = "Forbidden";
	_errorMessages[404] = "Not Found";
	_errorMessages[405] = "Method Not Allowed";
	_errorMessages[413] = "Payload Too Large";
	_errorMessages[414] = "URI Too Long";
	_errorMessages[415] = "Unsupported Media Type";
	_errorMessages[429] = "Too Many Requests";
	_errorMessages[500] = "Internal Server Error";
	_errorMessages[501] = "Not Implemented";
	_errorMessages[502] = "Bad Gateway";
	_errorMessages[503] = "Service Unavailable";
	_errorMessages[505] = "HTTP Version Not Supported";
}
