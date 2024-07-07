#include "Route.hpp"

Route::~Route()
{
	if (_cgi != NULL)
		delete _cgi;
}

void	Route::printRoute()
{
	std::cout << CYAN "ROUTE: " RST << std::endl;
	std::cout << GOLD "Root: " RST << _root << std::endl;
	std::cout << GOLD "Uri: " RST << _uri << std::endl;
	std::cout << GOLD "Method: " RST;
	for (size_t i = 0; i < _method.size(); i++)
		std::cout << _method[i] << " ";
	std::cout << std::endl;
	std::cout << GOLD "Index: " RST << _index << std::endl;
	std::cout << GOLD "Cgi enabled: " RST << _cgiEnabled << std::endl;
	if (_cgiEnabled)
	{
		std::cout << GOLD "Cgi path: " RST << _cgi->getPath() << std::endl;
		std::cout << GOLD "Cgi extension: " RST << _cgi->getExtension() << std::endl;
	}
	std::cout << GOLD "Is redirection: " RST << _isRedir << std::endl;
	std::cout << GOLD "Redirection path: " RST << _redirPath << std::endl;
}
