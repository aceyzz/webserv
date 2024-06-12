#include "Config.hpp"

inline int	errorReturn(std::string const &msg, const char *info)
{
	std::cerr << REDD "Error: " RST << YLLW << msg << " - " << info << RST << std::endl;
	return (1);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (errorReturn("Usage: ./webserv <config_file>", NULL));

	try
	{
		ParseConfig	parser(argv[1]);

		Webserver	webserv(parser.getConfigs());
		webserv.printConfigs();

		// init le serveur
		// lancer le serveur
	}
	catch (std::exception &e) { return (errorReturn("execption", e.what())); }

	return (0);
}
