#pragma once

#include "Webserv.hpp"

class	Cgi
{
	private:
		std::string		_path;
		std::string		_extension;

	public:
		Cgi() {};
		~Cgi() {};

		void	setPath(std::string path) { _path = path; };
		void	setExtension(std::string extension) { _extension = extension; };

		std::string	getPath() { return _path; };
		std::string	getExtension() { return _extension; };
};
