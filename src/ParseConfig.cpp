#include "ParseConfig.hpp"

ParseConfig::~ParseConfig()
{
	for (size_t i = 0; i < _configs.size(); i++)
	{
		if (_configs[i])
			delete _configs[i];
	}
}

void	ParseConfig::loadConfig(std::string path)
{
    std::ifstream	file(path.c_str());
    if (!file.is_open())
        throw std::runtime_error("Error: could not open file " + path);
    
    std::string					line;
    std::string					currentConfig;
    std::vector<std::string>	configs;

    while (std::getline(file, line))
	{
        if (line.empty() || line[0] == '#')
            continue;

        if (line.find("server {") != std::string::npos)
		{
            if (!currentConfig.empty())
                configs.push_back(currentConfig);
            currentConfig.clear();
        }
        currentConfig += line + "\n";
    }

    if (!currentConfig.empty())
        configs.push_back(currentConfig);
    
    file.close();

    for (size_t i = 0; i < configs.size(); i++)
        parseEachConfig(configs[i]);
}

void ParseConfig::parseEachConfig(std::string config)
{
    Config*	newConfig = new Config();
    
    std::istringstream			iss(config);
    std::string					line;
    std::vector<std::string>	locationBlocks;
    std::string					remainingConfig;

    while (std::getline(iss, line))
	{
        if (line.find("location") != std::string::npos)
		{
            std::string locationBlock = line + "\n";
            while (std::getline(iss, line) && line.find("}") == std::string::npos)
                locationBlock += line + "\n";
            locationBlock += line + "\n";
            locationBlocks.push_back(locationBlock);
        }
		else
            remainingConfig += line + "\n";
    }

    parseServerConfig(newConfig, remainingConfig);
    parseLocationBlocks(newConfig, locationBlocks);

    _configs.push_back(newConfig);
}

void	ParseConfig::parseServerConfig(Config* config, std::string remainingConfig)
{
    std::istringstream iss(remainingConfig);
    std::string line;

    while (std::getline(iss, line))
	{
        if (line.find("listen") != std::string::npos) {
            size_t pos = line.find("listen") + 6;
            try {
                config->setPort(std::stoi(line.substr(pos, line.find(";") - pos)));
            } catch (std::exception &e) {
                config->setPort(-1);
                continue;
            }
        } else if (line.find("name") != std::string::npos) {
            size_t pos = line.find("name") + 4;
            config->setName(trimWhitespaces(line.substr(pos, line.find(";") - pos)));
        } else if (line.find("root") != std::string::npos) {
            size_t pos = line.find("root") + 4;
            config->setRoot(trimWhitespaces(line.substr(pos, line.find(";") - pos)));
        } else if (line.find("index") != std::string::npos) {
            size_t pos = line.find("index") + 5;
            config->setIndex(trimWhitespaces(line.substr(pos, line.find(";") - pos)));
        } else if (line.find("error_page") != std::string::npos) {
            size_t pos = line.find("error_page") + 10;
            std::istringstream errorStream(line.substr(pos, line.find(";", pos) - pos));
            int code;
            std::string path;
            errorStream >> code >> path;
            config->addErrorPage(code, trimWhitespaces(path));
        } else if (line.find("max_body_size") != std::string::npos) {
            size_t pos = line.find("max_body_size") + 13;
            try {
                config->setMaxBodySize(std::stoi(line.substr(pos, line.find(";", pos) - pos)));
            } catch (std::exception &e) {
                config->setMaxBodySize(-1);
            }
        } else if (line.find("listing") != std::string::npos) {
            size_t pos = line.find("listing") + 8;
            config->setAutoindex(line.substr(pos, line.find(";", pos) - pos) == "on");
        }
    }
}

void	ParseConfig::parseLocationBlocks(Config* config, std::vector<std::string> locationBlocks)
{
    for (size_t i = 0; i < locationBlocks.size(); i++)
    {
        Route* 				route = new Route();
        Cgi* 				cgi = new Cgi();
        std::istringstream	iss(locationBlocks[i]);
        std::string			line;
        bool				isCgiBlock = false;

        while (std::getline(iss, line))
        {
            if (line.find("location") != std::string::npos) {
                size_t pos = line.find("location") + 8;
                route->setUri(trimWhitespaces(line.substr(pos, line.find("{") - pos)));
            } else if (line.find("root") != std::string::npos) {
                size_t pos = line.find("root") + 4;
                route->setRoot(trimWhitespaces(line.substr(pos, line.find(";") - pos)));
            } else if (line.find("methods") != std::string::npos) {
                size_t pos = line.find("methods") + 7;
                std::istringstream methodStream(line.substr(pos, line.find(";") - pos));
                std::string method;
                std::vector<std::string> methods;
                while (methodStream >> method)
                    methods.push_back(method);
                route->setMethod(methods);
            } else if (line.find("index") != std::string::npos) {
                size_t pos = line.find("index") + 5;
                route->setIndex(trimWhitespaces(line.substr(pos, line.find(";") - pos)));
            } else if (line.find("cgi_path") != std::string::npos) {
                size_t pos = line.find("cgi_path") + 8;
                cgi->setPath(trimWhitespaces(line.substr(pos, line.find(";", pos) - pos)));
                isCgiBlock = true;
            } else if (line.find("cgi_ext") != std::string::npos) {
                size_t pos = line.find("cgi_ext") + 7;
                cgi->setExtension(trimWhitespaces(line.substr(pos, line.find(";", pos) - pos)));
                isCgiBlock = true;
            }
        }

        if (isCgiBlock)
		{
            route->setCgiEnabled(true);
            route->setCgi(cgi);
        }
		else
            delete (cgi);

        // Appliquer les valeurs par défaut si elles ne sont pas définies
        if (route->getRoot().empty()) {
            route->setRoot(config->getRoot());
        }
        if (route->getIndex().empty()) {
            route->setIndex(config->getIndex());
        }
        config->addRoute(route);
    }
}

std::string	ParseConfig::trimWhitespaces(std::string str)
{
	std::string	result = str;

	result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
	result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);

	return (result);
}

void ParseConfig::printConfigs()
{
    for (size_t j = 0; j < _configs.size(); j++)
	{
        Config* config = _configs[j];
        std::cout << CYAN << "PRINT CONFIG (CLASS):" << RST << std::endl;
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
        }
        std::cout << std::endl;
    }
}
