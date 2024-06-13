#pragma once

#include "Webserv.hpp"

enum	RequestStatus
{
	RECEIVING,
	COMPLETE,
	ERROR
};

class	Request
{
	private:
		int									_fd;
		std::string							_rawRequest;
		std::string							_method;
		std::string							_uri;
		std::string							_versionHTTP;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_param;
		std::string							_clientIp;
		std::time_t							_timestamp;
		RequestStatus						_status;
		size_t								_bodySize;
	
	public:
		Request(int fd, std::string clientIp) : _fd(fd), _clientIp(clientIp), _status(RECEIVING), _bodySize(0) {};
		~Request() { _headers.clear(); };

		// Partie 1 : Parsing Orchestral
		void	parseRequest(std::string rawRequest);
		// Partie 2 : Parsing de la ligne de requête
		void	parseRequestLine(const std::string& line);
		// Partie 3 : Parsing des en-têtes et du corps
		void	parseHeadersAndBody(const std::vector<std::string>& lines);

		// Debug
		void	printRequest();
};
