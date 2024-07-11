#pragma once

#include "Webserv.hpp"

class	Logger
{
	private:
		std::string		_fileName;
		time_t			_time;
		std::ofstream	_file;
	
	public:
		Logger();
		Logger(const std::string &fileName);
		~Logger();

		void		log(const std::string &message);

		std::string		getFileName();
		std::ofstream&	getFile();
};
