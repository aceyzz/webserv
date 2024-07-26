#include "Config.hpp"
#include "Checker.hpp"

int	g_signal = 1;

inline int	errorReturn(std::string const &msg, const char *info)
{
	if (msg != "")
		std::cerr << REDD "Error: " RST << YLLW << msg << RST;
	else
		std::cerr << REDD "Error." RST;

	if (info)
		std::cerr << YLLW " - " << info << RST << std::endl;
	else
		std::cerr << std::endl;
	return (1);
}

inline void	signalHandler(int signum)
{
	std::string	signal;

	switch (signum)
	{
		case SIGINT:
			signal = "Ctrl+C";
			break;
		case SIGQUIT:
			signal = "ctrl+\\";
			break;
		case SIGTERM:
			signal = "kill";
			break;
		default:
			signal = "Unknown signal";
			break;
	}
	if (signum == SIGINT || signum == SIGQUIT || signum == SIGTERM)
	{
		std::cout << std::endl << std::endl;
		std::cout << BLUE "Signal received (" GOLD << signal << BLUE ") stopping the server..." RST << std::endl;
		g_signal = 0;
	}
}

void	customSignals()
{
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGPIPE, SIG_IGN);
}

int	main(int argc, char **argv)
{
	if (argc == 1)
	{
		createDefaultConfig();
		char *config = (char *)DEF_CONF_FILENAME;
		argv[1] = config;
	}
	else if (argc != 2)
		return (errorReturn("Usage: ./webserv <config_file>", NULL));

	try
	{
		Checker		check(argv[1]);
		ParseConfig	parser(argv[1]);
		Webserver	webserv(parser.getConfigs());
		
		if (DEBUG)
			webserv.printConfigs();

		customSignals();

		// init le serveur
		webserv.initServer();
		
		if (DEBUG)
			webserv.printSockets();
		
		webserv.runServer();
	}
	catch (std::exception &e) { return (errorReturn("execption", e.what())); }

	std::cout << LIME "Server stopped proprely. Good bye !" RST << std::endl << std::endl;
	return (0);
}
