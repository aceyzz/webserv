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

		// Getters
		int									getFD() const { return (_fd); };
		std::string							getRawRequest() const { return (_rawRequest); };
		std::string							getMethod() const { return (_method); };
		std::string							getUri() const { return (_uri); };
		std::string							getVersionHTTP() const { return (_versionHTTP); };
		std::map<std::string, std::string>	getHeaders() const { return (_headers); };
		std::string							getBody() const { return (_body); };
		std::string							getParam() const { return (_param); };
		std::string							getClientIp() const { return (_clientIp); };
		std::time_t							getTimestamp() const { return (_timestamp); };
		RequestStatus						getStatus() const { return (_status); };
		size_t								getBodySize() const { return (_bodySize); };

		// Setters
		void	setStatus(RequestStatus status) { _status = status; };
		void	setUri(std::string uri) { _uri = uri; };

		// Utils
		void	appendRawRequest(std::string rawRequest) { _rawRequest.append(rawRequest); };

		// Debug
		void	printRequest();
};
