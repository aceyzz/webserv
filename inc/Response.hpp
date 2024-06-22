#pragma once

#include "Webserv.hpp"

enum eResponseStatus
{
	WRITING = 69,
	READY,
	SENT
};

class	Response
{
	private:
		eResponseStatus						_status;
		
		Request*							_request;
		Config*								_config;
		Socket*								_clientSocket;

		int									_HTTPcode;
		std::string							_statusMessage;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		std::string							_resultResponse;

		size_t								_bytesSent;
		size_t								_totalBytes;

		size_t								_currentChunkOffset;
		std::string							_responseChunk;

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
		std::string							getStatusMessage() const { return (_statusMessage); };

		// Methods
		void	interpretRequest();
		void	sendResponse();

		// Orchestrales pour GET
		void	handleGet(const std::string &path);

		// Orchestrales pour DELETE
		void	handleDelete(const std::string &path);

		// Utils
		void	buildErrorPage(int errorCode);
		void	formatResponseToStr();
		void	buildListingPage();
		int		isFileOrDir(const std::string &str);
		bool	isAllowedMethod(const std::string &method, Route *route);
		bool	handleRequestTooLarge();
		bool	handleUriTooLarge(const std::string &uri);

		// Debug
		void	printResponse();

};
