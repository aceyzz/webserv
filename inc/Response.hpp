#pragma once

#include "Webserv.hpp"

enum eResponseStatus
{
	WRITING = 69,
	SENT
};

class	Response
{
	private:
		eResponseStatus						_status;
		int									_HTTPcode;
		Request*							_request;
		Config*								_config;
		Socket*								_clientSocket;

		std::string							_resultResponse;
		size_t								_bytesSent;
		std::map<std::string, std::string>	_headers;
		std::string							_body;

	public:
		Response(Request* request, Config* config, Socket* clientSocket);
		~Response() { _headers.clear(); };

		// Getters
		eResponseStatus						getStatus() const { return (_status); };
		int									getHTTPcode() const { return (_HTTPcode); };
		std::string							getResultResponse() const { return (_resultResponse); };
		size_t								getBytesSent() const { return (_bytesSent); };
		std::map<std::string, std::string>	getHeaders() const { return (_headers); };
		std::string							getBody() const { return (_body); };

		// Debug
		void	printResponse();

};
