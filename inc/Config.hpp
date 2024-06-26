#pragma once

#include "Webserv.hpp"

class	Config
{
	private:
		std::string						_name;
		int								_port;
		std::string						_root;
		std::string						_index;
		std::map<int, std::string>		_errorPages;
		bool							_autoindex;
		std::map<std::string, Route*>	_routes;
		int								_maxBodySize;
		std::map<int, std::string>		_errorMessages;

	public:
		Config() : _port(-1), _autoindex(false), _maxBodySize(-1) { setErrorMessages(); };
		~Config();
		// Getters
		std::string						getName() { return _name; };
		int								getPort() { return _port; };
		std::string						getRoot() { return _root; };
		std::string						getIndex() { return _index; };
		std::map<int, std::string>		getErrorPages() { return _errorPages; };
		std::string						getErrorPage(int code);
		bool							getAutoindex() { return _autoindex; };
		std::map<std::string, Route*>	getRoutes() { return _routes; };
		Route*							getRoute(std::string uri);
		int								getMaxBodySize() { return _maxBodySize; };
		std::string						getErrorMessage(int code) { return _errorMessages[code]; };

		// Setters
		void	setName(std::string name) { _name = name; };
		void	setPort(int port) { _port = port; };
		void	setRoot(std::string root) { _root = root; };
		void	setIndex(std::string index) { _index = index; };
		void	addErrorPage(int code, std::string path) { _errorPages[code] = path; };
		void	setAutoindex(bool autoindex) { _autoindex = autoindex; };
		void	addRoute(Route* route);
		void	setMaxBodySize(int maxBodySize) { _maxBodySize = maxBodySize; };
		void	setErrorMessages();

		// Debug
		void	printConfig();
};
