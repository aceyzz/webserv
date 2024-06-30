#pragma once

#include "Webserv.hpp"

class	CgiHandler
{
	private:
		int			_kqueue;
		Route*		_route;
		Request*	_request;
		Response*	_response;
		Config*		_config;

		char**		_envp;
		char**		_args;

		std::string	_responseLine;
		std::string	_cgiOutputResult;
		int			_pipeFd[2];

		bool		_cgiOutputReady;

		void	initEnvp();
		void	initArgs();
		void	initPipe();

	public:
		CgiHandler(Route* route, Request* request, Response* response, Config* config, int kqueue);
		~CgiHandler();

		// Getters
		std::string	getResponseLine() { return _responseLine; };
		std::string	getCgiOutputResult() { return _cgiOutputResult; };
		bool		getCgiOutputReady() { return _cgiOutputReady; };

		// Setters
		void	setCgiOutputReady(bool ready) { _cgiOutputReady = ready; };

		// Debug
		void	printCgiHandler();
};
