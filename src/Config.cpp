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
