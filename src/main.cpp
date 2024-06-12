#include "Config.hpp"

int	main(int argc, char **argv)
{
	ParseConfig	config;

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
		return (1);
	}

	config.loadConfig("config/config.conf");

	config.printConfigs();

	return (0);
}
