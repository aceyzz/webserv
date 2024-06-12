#include "Config.hpp"

int	main(int argc, char **argv)
{
	ParseConfig	config;

	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
		return (1);
	}

	try
	{
		config.loadConfig("config/config.conf");
		config.printConfigs();
	}
	catch (std::exception &e)
	{
		std::cerr << REDD "Error: " RST << e.what() << std::endl;
		return (1);
	}

	return (0);
}
