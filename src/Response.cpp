#include "Response.hpp"

Response::Response(Request* request, Config* config, Socket* clientSocket)
{
	_status = WRITING;
	_HTTPcode = 0;
	_request = request;
	_config = config;
	_clientSocket = clientSocket;
	_bytesSent = -1;
	_body = "";
	_resultResponse = "";
}

void	Response::printResponse()
{
	std::cout << std::endl;
	std::cout << GOLD "- RESPONSE -" RST << std::endl;
	std::cout << CYAN "Status: " RST;
	if (_status == WRITING)
		std::cout << "WRITING" << std::endl;
	else if (_status == READY)
		std::cout << "READY" << std::endl;
	else if (_status == SENT)
		std::cout << "SENT" << std::endl;
	std::cout << CYAN "Adress of request: " RST << _request << std::endl;
	std::cout << CYAN "Adress of config: " RST << _config << std::endl;
	std::cout << CYAN "Adress of client socket: " RST << _clientSocket << std::endl;
	std::cout << CYAN "HTTP code: " RST << _HTTPcode << std::endl;
	std::cout << CYAN "Status message: " RST << _statusMessage << std::endl;
	std::cout << CYAN "Headers: " RST << std::endl;
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << CYAN "Body: " RST << _body << std::endl;
	std::cout << std::endl;
	std::cout << GOLD "- RESULT RESPONSE -" RST << std::endl;
	std::cout << _resultResponse << std::endl;
	std::cout << std::endl;
	if (_status == SENT)
		std::cout << CYAN "Bytes sent: " RST << _bytesSent << std::endl;
}

void	Response::interpretRequest()
{
	std::string	method = _request->getMethod();
	std::string	uri = _request->getUri();

	Route*	route = _config->getRoute(uri);
	if (!route)
	{
		buildErrorPage(404);
		return ;
	}

	// HANDLE GET
	// HANDLE POST
	// HANDLE DELETE
	// Sinon buildErrorPage(405);

	// Structurer la reponse en string a partir des datas de la classe pour envoi final
	if (_status == READY)
		formatResponseToStr();
}

void	Response::buildErrorPage(int errorCode)
{
	std::map<int, std::string>	errorPages = _config->getErrorPages();
	if (errorPages.find(errorCode) != errorPages.end())
	{
		std::string		errorPagePath = errorPages[errorCode];
		std::ifstream	file(errorPagePath.c_str());
		if (file.is_open())
		{
			std::string	content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			_body = content;
		}
		else
			_body = strToHtml(_config->getErrorMessage(errorCode));
	}
	_HTTPcode = errorCode;
	_statusMessage = _config->getErrorMessage(errorCode);
	_status = READY;
}

void	Response::formatResponseToStr()
{
	_resultResponse = HTTP_VERSION;
	_resultResponse += " ";
	_resultResponse += std::to_string(_HTTPcode);
	_resultResponse += " ";
	_resultResponse += _statusMessage;
	_resultResponse += "\r\n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		_resultResponse += it->first;
		_resultResponse += ": ";
		_resultResponse += it->second;
		_resultResponse += "\r\n";
	}
	_resultResponse += "\r\n";
	_resultResponse += _body;
}

std::string	strToHtml(std::string content)
{
	std::string	html = "<html>\n";
	html += "<head>\n";
	html += "<title>Webserver</title>\n";
	html += "</head>\n";
	html += "<body>\n";
	html += "<h1>";
	html += content;
	html += "</h1>\n";
	html += "</body>\n";
	html += "</html>\n";
	return (html);
}
