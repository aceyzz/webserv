#include "Config.hpp"

int	g_signal = 1;

inline int	errorReturn(std::string const &msg, const char *info)
{
	std::cerr << REDD "Error: " RST << YLLW << msg << " - " << info << RST << std::endl;
	return (1);
}

inline void	signalHandler(int signum)
{
	std::string	signal;

	switch (signum)
	{
		case SIGINT:
			signal = "ctrl-c";
			break;
		case SIGQUIT:
			signal = "ctrl-\\";
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
}

int	main(int argc, char **argv)
{

	sleep(10);

	if (argc != 2)
		return (errorReturn("Usage: ./webserv <config_file>", NULL));

	try
	{
		ParseConfig	parser(argv[1]);

		Webserver	webserv(parser.getConfigs());
		// webserv.printConfigs();

		customSignals();

		// init le serveur
		webserv.initServer();
		// webserv.printSockets();
		webserv.runServer();
	}
	catch (std::exception &e) { return (errorReturn("execption", e.what())); }

	std::cout << CYAN "Server stopped proprely. Good bye !" RST << std::endl << std::endl;

	return (0);
}
