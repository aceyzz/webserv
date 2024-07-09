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

void Request::parseRequest(const std::vector<char>& rawRequestData)
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
			parseHeadersAndBody(lines, rawRequestData);  // Parser les en-têtes et le corps
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

void Request::parseHeadersAndBody(const std::vector<std::string>& lines, const std::vector<char>& rawRequestData)
{
    size_t lineIndex = 1;
    size_t byteOffset = lines[0].length() + 1;  // +1 pour le caractère de nouvelle ligne

    // Calculer la taille totale des en-têtes pour trouver le début du corps
    for (; lineIndex < lines.size(); ++lineIndex)
    {
        const std::string& line = lines[lineIndex];
        byteOffset += line.length() + 1;  // +1 pour le caractère de nouvelle ligne

        if (line.empty()) // Ligne vide trouvée, fin des en-têtes
        {
            byteOffset++;  // Passer le caractère de nouvelle ligne supplémentaire après les en-têtes
            break;  // Sortir de la boucle car les en-têtes sont terminés
        }

        size_t delimiter = line.find(": ");
        if (delimiter != std::string::npos)
        {
            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 2);
            _headers[key] = value;
        }
    }

    // Ajustement pour passer la ligne vide qui sépare les en-têtes du corps
    if (lineIndex < lines.size() && lines[lineIndex].empty())
    {
        byteOffset++;  // Passer la ligne vide (sur les systèmes où la fin des en-têtes est marquée par "\n\n")
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
