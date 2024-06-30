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

		void	initEnvp();
		void	initArgs();
		void	initPipe();

	public:
		CgiHandler(Route* route, Request* request, Response* response, Config* config, int kqueue);
		~CgiHandler();

		void	printCgiHandler();
};
