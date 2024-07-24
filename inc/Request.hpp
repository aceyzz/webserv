#pragma once

#include "Webserv.hpp"

enum	RequestStatus
{
	RECEIVING,
	COMPLETE,
	ERROR,
	CONTINUE
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

		bool								_continueSent;

	public:
		Request(int fd, std::string clientIp) : _fd(fd), _clientIp(clientIp), _status(RECEIVING), _bodySize(0), _continueSent(false) {};
		~Request() { _headers.clear(); };

		// Partie 1 : Parsing Orchestral
		void	parseRequest(const std::vector<char>& rawRequestData);
		// Partie 2 : Parsing de la ligne de requête
		void	parseRequestLine(const std::string& line);
		// Partie 3 : Parsing des en-têtes et du corps
		void	parseHeaders(const std::string& rawRequestData);

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
		bool								getContinueSent() const { return (_continueSent); };

		// Setters
		void	setStatus(RequestStatus status) { _status = status; };
		void	setUri(std::string uri) { _uri = uri; };
		void	setBody(std::string body) { _body = body; };
		void	setRawRequest(std::string rawRequest) { _rawRequest = rawRequest; };
		void	setContinueSent(bool continueSent) { _continueSent = continueSent; };

		// Utils
		void	appendRawRequest(std::string rawRequest) { _rawRequest.append(rawRequest); };
		bool	expectsContinue();
		void	parseParamsUri();

		// Debug
		void	printRequest();
};
