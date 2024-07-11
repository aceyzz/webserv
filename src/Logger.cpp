#include "Logger.hpp"

Logger::Logger()
{
	_fileName = DEFAULT_LOG;
	_file.open(_fileName.c_str(), std::ios::out | std::ios::app);
}

Logger::Logger(const std::string &fileName)
{
	_fileName = fileName;
	_file.open(_fileName.c_str(), std::ios::out | std::ios::app);
}

Logger::~Logger()
{
	_file.close();
}

void	Logger::log(const std::string &message)
{
	_time = time(0);
	std::string timestr = ctime(&_time);
	// remove the \n of the timestr
	timestr.pop_back();

	_file << "[" << timestr << "] " << message << std::endl;
}

std::string	Logger::getFileName()
{
	return (_fileName);
}

std::ofstream&	Logger::getFile()
{
	return (_file);
}
