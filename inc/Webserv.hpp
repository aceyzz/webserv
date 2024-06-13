#pragma once

#include "lib.hpp"

extern int g_signal;

class	ParseConfig;
class	Config;
class	Route;
class	Cgi;
class	Socket;
class	Request;

#include "ParseConfig.hpp"
#include "Config.hpp"
#include "Cgi.hpp"
#include "Route.hpp"
#include "Socket.hpp"
#include "Request.hpp"

class	Webserver
{
	private:
		int						_kqueue;
		std::vector<Config*>	_configs;
		std::vector<Socket*>	_serverSockets;
		std::vector<Socket*>	_clientSockets;
		std::map<int, Request*>	_requests;
	
	public:
		// Constructors & destructors
		Webserver(std::vector<Config*> config) : _kqueue(-1), _configs(config)  {};
		~Webserver();

		// Getters
		std::vector<Config*>	getServersConfig() { return (_configs); };
		std::vector<Socket*>	getServerSockets() { return (_serverSockets); };
		std::vector<Socket*>	getClientSockets() { return (_clientSockets); };
		std::map<int, Request*>	getRequests() { return (_requests); };
		Request					*getRequest(int fd) { return (_requests[fd]); };

		// Setters
		void	setServerSockets(std::vector<Socket*> sockets) { _serverSockets = sockets; };
		void	setClientSockets(std::vector<Socket*> sockets) { _clientSockets = sockets; };
		void	addServerSocket(Socket *socket) { _serverSockets.push_back(socket); };
		void	addClientSocket(Socket *socket) { _clientSockets.push_back(socket); };

		// Methods
		void	initServer();
		void	runServer();

		// Orchestrales pour Request
		void	handleRequest(int fd);

		// Debug
		void	printConfigs();
		void	printSockets();
};
