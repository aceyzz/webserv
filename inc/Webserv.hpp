#pragma once

#include "lib.hpp"

extern int g_signal;

class	ParseConfig;
class	Config;
class	Route;
class	Cgi;
class	Socket;
class	Request;
class	Response;

#include "ParseConfig.hpp"
#include "Config.hpp"
#include "Cgi.hpp"
#include "Route.hpp"
#include "Socket.hpp"
#include "Request.hpp"
#include "Response.hpp"

class	Webserver
{
	private:
		int							_kqueue;
		std::vector<Config*>		_configs;
		std::map<int, Socket*>		_serverSockets;
		std::map<int, Socket*>		_clientSockets;
		std::map<int, Request*>		_requests;
		std::map<int, Response*>	_responses;
		std::map<int, Config*>		_configsByPort;
	
	public:
		// Constructors & destructors
		Webserver(std::vector<Config*> config);
		~Webserver();

		// Getters
		std::vector<Config*>	getServersConfig() { return (_configs); };
		std::map<int, Socket*>	getServerSockets() { return (_serverSockets); };
		std::map<int, Socket*>	getClientSockets() { return (_clientSockets); };
		std::map<int, Request*>	getRequests() { return (_requests); };
		Request					*getRequest(int fd) { return (_requests[fd]); };

		// Setters
		void	setServerSockets(std::map<int, Socket*> sockets) { _serverSockets = sockets; };
		void	setClientSockets(std::map<int, Socket*> sockets) { _clientSockets = sockets; };
		void	addServerSocket(Socket *socket);
		void	addClientSocket(Socket *socket);

		// Methods
		void	initServer();
		void	runServer();

		// Orchestrales pour Request
		void	parseAndHandleRequest(int fd);
		// Methodes pour Request
		bool	receiveRequest(int fd);

		// Clean
		void	closeClient(int fd);

		// Orchestrales pour Response
		bool	responseManager(int clientFD);

		// handleContinue
		void	sendContinueResponse(int clientFD);

		// Utils
		bool	isServerSocket(int fd);
		void	acceptNewClient(int serverFD);
		Config*	getConfigForClient(int clientFD);

		// Debug
		void	printConfigs();
		void	printSockets();
		void	printConfigByPort();
};
