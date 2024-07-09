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
		std::istringstream stream(rawData);
		std::string line;
		std::vector<std::string> lines;

		while (std::getline(stream, line, '\n'))
		{
			if (!line.empty() && line.back() == '\r')
				line.pop_back();
			lines.push_back(line);
		}

		if (!lines.empty())
		{
			parseRequestLine(lines[0]);  // Parser la ligne de requête
			parseHeadersAndBody(rawRequestData);  // Parser les en-têtes et le corps
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
	}
}

void	Request::parseRequestLine(const std::string& line)
{
	std::istringstream	lineStream(line);
	lineStream >> _method >> _uri >> _versionHTTP;

	// MAJ du statut
	if (_method.empty() || _uri.empty() || _versionHTTP.empty())
		_status = ERROR;
	else
		_status = RECEIVING;

	// Mettre à jour le timestamp au moment de la réception de la requête
	_timestamp = std::time(nullptr);

	parseParamsUri();
}

void	Request::parseHeadersAndBody(const std::vector<char>& rawRequestData)
{
	size_t byteOffset = 0;
	size_t rawSize = rawRequestData.size();

	// Trouver la fin de la première ligne (ligne de requête)
	size_t lineEnd = std::string::npos;
	for (size_t i = 0; i < rawSize - 1; ++i)
	{
		if (rawRequestData[i] == '\r' && rawRequestData[i + 1] == '\n')
		{
			lineEnd = i;
			break;
		}
	}

	if (lineEnd == std::string::npos)
	{
		_status = ERROR;
		return;
	}

	// Parser la ligne de requête
	std::string requestLine(rawRequestData.begin(), rawRequestData.begin() + lineEnd);
	parseRequestLine(requestLine);
	byteOffset = lineEnd + 2;  // Sauter "\r\n"

	// Parser les en-têtes
	while (byteOffset < rawSize)
	{
		lineEnd = std::string::npos;
		for (size_t i = byteOffset; i < rawSize - 1; ++i)
		{
			if (rawRequestData[i] == '\r' && rawRequestData[i + 1] == '\n')
			{
				lineEnd = i;
				break;
			}
		}

		if (lineEnd == std::string::npos)
		{
			_status = ERROR;
			return;
		}

		// Ligne vide trouvée, fin des en-têtes
		if (lineEnd == byteOffset)
		{
			byteOffset += 2;  // Sauter "\r\n"
			break;
		}

		std::string headerLine(rawRequestData.begin() + byteOffset, rawRequestData.begin() + lineEnd);
		byteOffset = lineEnd + 2;  // Sauter "\r\n"

		size_t delimiter = headerLine.find(": ");
		if (delimiter != std::string::npos)
		{
			std::string key = headerLine.substr(0, delimiter);
			std::string value = headerLine.substr(delimiter + 2);
			_headers[key] = value;
		}
	}

	// S'assurer que le corps est traité correctement en commençant à partir du byteOffset correct
	if (byteOffset < rawRequestData.size())
	{
		_body.assign(rawRequestData.begin() + byteOffset, rawRequestData.end());
		_bodySize = _body.size();
	}
	else
	{
		_body.clear();
		_bodySize = 0;
	}
	_status = COMPLETE;
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
