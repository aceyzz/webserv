#pragma once

#include "Webserv.hpp"

class	Route
{
	private:
		std::string					_root;
		std::string					_uri;
		std::vector<std::string>	_method;
		std::string					_index;
		bool						_cgiEnabled;
		Cgi							*_cgi;
	
	public:
		Route() : _cgiEnabled(false), _cgi(nullptr) {};
		~Route();

		void	setRoot(std::string root) { _root = root; };
		void	setUri(std::string uri) { _uri = uri; };
		void	setMethod(std::vector<std::string> method) { _method = method; };
		void	setIndex(std::string index) { _index = index; };
		void	setCgiEnabled(bool cgiEnabled) { _cgiEnabled = cgiEnabled; };
		void	setCgi(Cgi *cgi) { _cgi = cgi; };

		std::string					getRoot() { return _root; };
		std::string					getUri() { return _uri; };
		std::vector<std::string>	getMethod() { return _method; };
		std::string					getIndex() { return _index; };
		bool						getCgiEnabled() { return _cgiEnabled; };
		Cgi							*getCgi() { return _cgi; };
		std::string					getCgiPath() { return _cgi->getPath(); };
		std::string					getCgiExtension() { return _cgi->getExtension(); };
};
