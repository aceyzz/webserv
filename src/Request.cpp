#include "Request.hpp"

void	Request::printRequest()
{
	std::cout << GOLD "- REQUEST - " << std::endl;
	std::cout << CYAN "- RawRequest:\n" RST << _rawRequest << std::endl;
	std::cout << CYAN "- FD: " RST << _fd << std::endl;
	std::cout << CYAN "- Method: " RST << _method << std::endl;
	std::cout << CYAN "- Uri: " RST << _uri << std::endl;
	std::cout << CYAN "- Param: " RST << _param << std::endl;
	std::cout << CYAN "- Version HTTP: " RST << _versionHTTP << std::endl;
	std::cout << CYAN "- Headers: " RST << std::endl;
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	std::cout << CYAN "- Body: " RST << _body << std::endl;
	std::cout << CYAN "- Client IP: " RST << _clientIp << std::endl;
	std::cout << CYAN "- Timestamp: " RST << _timestamp << std::endl;
	std::cout << CYAN "- Status: " RST << _status << std::endl;
	std::cout << CYAN "- Body size: " RST << _bodySize << std::endl;
}

void	Request::parseRequest(const std::vector<char>& rawRequestData)
{
	try
	{
		// Convertir en string pour parser les lignes d'en-tête
		std::string rawData(rawRequestData.begin(), rawRequestData.end());
		const char* rawPtr = rawData.c_str();

		// Trouver la fin de la première ligne (ligne de requête)
		const char* lineEnd = strstr(rawPtr, "\r\n");
		if (lineEnd == nullptr)
		{
			_status = ERROR;
			return;
		}

		// Parser la ligne de requête
		std::string requestLine(rawPtr, lineEnd - rawPtr);
		parseRequestLine(requestLine);

		// Avancer le pointeur après la ligne de requête
		rawPtr = lineEnd + 2;

		// Trouver la fin des en-têtes
		const char* headerEnd = strstr(rawPtr, "\r\n\r\n");
		if (headerEnd == nullptr)
		{
			_status = ERROR;
			return;
		}

		// Parser les en-têtes
		std::string headers(rawPtr, headerEnd - rawPtr);
		parseHeaders(headers);

		// Récupérer le corps de la requête
		rawPtr = headerEnd + 4;
		_body.assign(rawPtr, rawData.c_str() + rawData.size() - rawPtr);
		_bodySize = _body.size();

		_status = COMPLETE;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
		_status = ERROR;
	}
}

void	Request::parseRequestLine(const std::string& line)
{
	std::istringstream lineStream(line);
	lineStream >> _method >> _uri >> _versionHTTP;

	// MAJ du statut
	if (_method.empty() || _uri.empty() || _versionHTTP.empty())
		_status = ERROR;
	else
		_status = RECEIVING;

	// Mettre à jour le timestamp au moment de la réception de la requête
	_timestamp = std::time(NULL);

	parseParamsUri();
}

void	Request::parseHeaders(const std::string& headers)
{
	std::istringstream headerStream(headers);
	std::string line;
	while (std::getline(headerStream, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		size_t delimiter = line.find(": ");
		if (delimiter != std::string::npos)
		{
			std::string key = line.substr(0, delimiter);
			std::string value = line.substr(delimiter + 2);
			_headers[key] = value;
		}
	}
}

bool	Request::expectsContinue()
{
	std::map<std::string, std::string>::iterator it = _headers.find("Expect");
	if (it != _headers.end() && it->second == "100-continue")
		return (true);
	return (false);
}

void	Request::parseParamsUri()
{
	size_t pos = _uri.find("?");
	if (pos != std::string::npos)
	{
		_param = _uri.substr(pos + 1);
		_uri = _uri.substr(0, pos);
	}
}
