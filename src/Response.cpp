#include "Response.hpp"

Response::Response(Request* request, Config* config, Socket* clientSocket)
{
	_status = WRITING;
	_HTTPcode = 200;
	_request = request;
	_config = config;
	_clientSocket = clientSocket;
	_bytesSent = 0;
	_body = "";
	_resultResponse = "HTTP/1.1 ";
	_headers["Server"] = "webserv/1.0";
	_headers["Date"] = "Sun, 06 Nov 1994 08:49:37 GMT";
	_headers["Content-Type"] = "text/html";
	_headers["Connection"] = "close";
}

void	Response::printResponse()
{
	std::cout << std::endl;
	std::cout << GOLD "- RESPONSE -" RST << std::endl;
	std::cout << CYAN "Status: " RST << _status << std::endl;
	std::cout << CYAN "Adress of request: " RST << _request << std::endl;
	std::cout << CYAN "Adress of config: " RST << _config << std::endl;
	std::cout << CYAN "Adress of client socket: " RST << _clientSocket << std::endl;
	std::cout << CYAN "HTTP code: " RST << _HTTPcode << std::endl;
	std::cout << CYAN "Result response: " RST << _resultResponse << std::endl;
	std::cout << CYAN "Bytes sent: " RST << _bytesSent << std::endl;
	std::cout << CYAN "Headers: " RST << std::endl;
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << CYAN "Body: " RST << _body << std::endl;
	std::cout << std::endl;
}
