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

void	Request::parseRequest(std::string rawRequest)
{
	_rawRequest = rawRequest;

	std::istringstream stream(rawRequest);
	std::string line;
	std::vector<std::string> lines;

	while (std::getline(stream, line))
	{
		// Enlever les retours de chariot potentiels
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		lines.push_back(line);
	}

	if (!lines.empty())
	{
		parseRequestLine(lines[0]); // parser la ligne de requête
		parseHeadersAndBody(lines); // parser les en-têtes et le corps
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
	_timestamp = std::time(nullptr);

	parseParamsUri();
}

void	Request::parseHeadersAndBody(const std::vector<std::string>& lines)
{
	size_t i = 1; // Ligne suivante après la ligne de requête

	// Parser les en-têtes
	for (; i < lines.size(); ++i)
	{
		const std::string& line = lines[i];

		if (line.empty())
		{
			++i;
			break;
		}

		size_t delimiter = line.find(": ");
		if (delimiter != std::string::npos)
		{
			std::string key = line.substr(0, delimiter);
			std::string value = line.substr(delimiter + 2);
			_headers[key] = value;
		}
	}

	// Parser le corps
	std::ostringstream bodyStream;
	for (; i < lines.size(); ++i)
	{
		bodyStream << lines[i];
		if (i != lines.size() - 1)
			bodyStream << "\n";
	}
	_body = bodyStream.str();
	_bodySize = _body.size();

	// MAJ du statut
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
