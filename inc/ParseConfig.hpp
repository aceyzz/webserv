#pragma once

#include "Webserv.hpp"

class	ParseConfig
{
	private:
		std::vector<Config*>	_configs;
	
	public:
		ParseConfig() {};
		~ParseConfig();
		// Orchestre le parsing de chaque serveur
		void	loadConfig(std::string path);
		// Parse chaque serveur
		void	parseEachConfig(std::string config);
		// Utils parsing
		void	parseLocationBlocks(Config* config, std::vector<std::string> locationBlocks);
		void	parseServerConfig(Config* config, std::string remainingConfig);
		// Utils secondaires
		std::string	trimWhitespaces(std::string str);
		// DEBUG
		void	printConfigs();
};
