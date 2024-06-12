#pragma once

#include "Webserv.hpp"

class	ParseConfig
{
	private:
		std::vector<Config*>	_configs;
	
	public:
		ParseConfig() {};
		~ParseConfig();

		// PARTIE 1 : Parsing du fichier de configuration (orchestral)
		void	loadConfig(std::string path);
		// PARTIE 2 : Parsing des configurations (individuel)
		void	parseEachConfig(std::string config);
		// PARTIE 3 : Parsing des blocs de configuration (individuel)
		void	parseLocationBlocks(Config* config, std::vector<std::string> locationBlocks);
		void	parseServerConfig(Config* config, std::string remainingConfig);

		// PARTIE 4 : Check de toutes les configurations (orchestral)
		void	checkConfigs();
		// PARTIE 5 : Check des configurations (individuel)
		void	checkServerConfig(Config* config);
		void	checkRouteConfig(Route *route);

		// Utils secondaires
		std::string	trimWhitespaces(std::string str);

		// Getters
		std::vector<Config*>	getConfigs() const { return _configs; };
		// DEBUG
		void	printConfigs();
};
