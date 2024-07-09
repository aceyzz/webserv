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
		int			_pipeFdCgi[2];

		bool		_cgiOutputReady;
		bool		_cgiLaunched;

		pid_t		_cgiPid;

		// Gros fichiers a envoyer au STDIN du cgi:
		// - le parent ecrit morceaux par morceaux dans le pipe du STDIN du cgi
		// - le cgi lit morceaux par morceaux dans son STDIN
		// - le cgi ecrit en entier dans son STDOUT (? ou dans un fichier temporaire ?)
		// - le parent lit morceaux par morceaux dans le pipe de son STDIN
		size_t		_bytesWritten;

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
		bool		getCgiLaunched() { return _cgiLaunched; };
		// Setters
		void	setCgiOutputReady(bool status) { _cgiOutputReady = status; };
		void	setCgiLaunched(bool status) { _cgiLaunched = status; };
		// Debug
		void	printCgiHandler();

		// Methods
		void	handleCgi();

		// Utils
		std::string	extractContentTypeCgiOutput();

		bool	isMultipart();
		void	extractMultipartData();
};
