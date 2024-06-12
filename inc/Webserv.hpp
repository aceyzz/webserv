#pragma once

#include "lib.hpp"

class	ParseConfig;
class	Config;
class	Route;
class	Cgi;

#include "ParseConfig.hpp"
#include "Config.hpp"
#include "Cgi.hpp"
#include "Route.hpp"

class	Webserver
{
	private:
		std::vector<Config*>	_configs;
	
	public:
		// Constructors & destructors
		Webserver(std::vector<Config*> config) : _configs(config) {};
		~Webserver() {};

		// Getters
		std::vector<Config*>	getConfigs() { return (_configs); };

		// Debug
		void	printConfigs();
};
