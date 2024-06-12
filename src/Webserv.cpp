#include "Webserv.hpp"

void	Webserver::printConfigs()
{
	for (size_t j = 0; j < _configs.size(); j++)
	{
        Config* config = _configs[j];
        std::cout << std::endl;
        std::cout << CYAN << "PRINT CONFIG (CLASS WEBSERVER):" << RST << std::endl;
		std::cout << GRY2 "Address of config: " << config << std::endl;
        std::cout << YLLW << "Name: " << RST << config->getName() << std::endl;
        std::cout << YLLW << "Port: " << RST << config->getPort() << std::endl;
        std::cout << YLLW << "Root: " << RST << config->getRoot() << std::endl;
        std::cout << YLLW << "Index: " << RST << config->getIndex() << std::endl;
        std::cout << YLLW << "Error pages: " << RST << std::endl;
		std::map<int, std::string> errorPages = config->getErrorPages();
		for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
            std::cout << "  " << it->first << " -> " << it->second << std::endl;
        std::cout << YLLW << "Autoindex: " << RST << (config->getAutoindex() ? "true" : "false") << std::endl;
        std::cout << YLLW << "Max body size: " << RST << config->getMaxBodySize() << std::endl;
        std::cout << YLLW << "Routes: " << RST << std::endl;
        std::vector<Route*> routes = config->getRoutes();
        for (size_t i = 0; i < routes.size(); i++)
		{
            Route* route = routes[i];
            std::cout << "  " << GRY1 "Route [" << i+1 << "] " RST << std::endl;
            std::cout << "    " << YLLW << "Root: " << RST << route->getRoot() << std::endl;
            std::cout << "    " << YLLW << "Uri: " << RST << route->getUri() << std::endl;
            std::cout << "    " << YLLW << "Methods: " << RST << std::endl;
            std::vector<std::string> methods = route->getMethod();
            for (size_t k = 0; k < methods.size(); k++)
                std::cout << "      " << methods[k] << std::endl;
            std::cout << "    " << YLLW << "Index: " << RST << route->getIndex() << std::endl;
			std::cout << "    " << YLLW << "Cgi enabled: " << RST << (route->getCgiEnabled() ? "true" : "false") << std::endl;
			if (route->getCgiEnabled())
			{
				std::cout << "    " << YLLW << "Cgi path: " << RST << route->getCgiPath() << std::endl;
				std::cout << "    " << YLLW << "Cgi extension: " << RST << route->getCgiExtension() << std::endl;
			}
			std::cout << "    " << YLLW << "Is redir: " << RST << (route->getIsRedir() ? "true" : "false") << std::endl;
			if (route->getIsRedir())
				std::cout << "    " << YLLW << "Redir path: " << RST << route->getRedirPath() << std::endl;
        }
        std::cout << std::endl;
    }
}
