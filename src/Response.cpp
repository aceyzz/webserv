#include "Response.hpp"

Response::Response(Request* request, Config* config, Socket* clientSocket, int kqueue)
{
	_status = WRITING;
	_HTTPcode = 0;
	_request = request;
	_config = config;
	_clientSocket = clientSocket;
	_bytesSent = -1;
	_totalBytes = 0;
	_body = "";
	_resultResponse = "";
	_currentChunkOffset = 0;
	_responseChunk = "";
	_kqueue = kqueue;
	_cgiHandler = NULL;
	_streamer = false;
	_fileStream = NULL;
	_cookie = "";
}

Response::~Response()
{
	if (_headers.size() > 0)
		_headers.clear();
	if (_cgiHandler)
	{
		delete _cgiHandler;
		_cgiHandler = NULL;
	}
	if (_fileStream != NULL)
	{
		if (_fileStream->is_open())
			_fileStream->close();
		if (_fileStream)
		{
			delete _fileStream;
			_fileStream = NULL;
		}
	}
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
			_body = strToHtml(std::to_string(errorCode) + " " + _config->getErrorMessage(errorCode));
	}
	_HTTPcode = errorCode;
	_statusMessage = _config->getErrorMessage(errorCode);
	_status = READY;

	// Si le content type n'est pas défini, on le met en text/html
	if (_headers.find("Content-Type") == _headers.end())
		_headers["Content-Type"] = "text/html";
}

void	Response::formatResponseToStr()
{
	// Construction de la première ligne de la réponse
	_resultResponse = HTTP_VERSION;
	_resultResponse += " ";
	_resultResponse += std::to_string(_HTTPcode);
	_resultResponse += " ";
	_resultResponse += _statusMessage;
	_resultResponse += "\r\n";

	// Ajout du cookie si présent
	std::string cookie = _request->getHeaders()["Cookie"];
	if (cookie.empty())
		generateAndSetCookie();
	else
		_headers["Cookie"] = cookie;

	// Ajout des autres headers
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		_resultResponse += it->first;
		_resultResponse += ": ";
		_resultResponse += it->second;
		_resultResponse += "\r\n";
	}
	_resultResponse += "\r\n";

	// Ajout du body
	_resultResponse += _body;
	_currentChunkOffset = 0;
}

void	Response::buildListingPage()
{
	std::string	uri = _request->getUri();
	std::string	path = _config->getRoot() + uri;

	std::map<std::string, std::string>	dirContent = getDirContent(path);
	std::string	html = "<html>\n";
	html += "<head>\n<title>Listing</title>\n</head>\n<body>\n<h1>Listing of ";
	html += uri;
	html += "</h1>\n<ul>\n";
	for (std::map<std::string, std::string>::iterator it = dirContent.begin(); it != dirContent.end(); it++)
	{
		html += "<li><a href=\"";
		html += uri;// pour reprendre le chemin
		if (uri.back() != '/')// si besoin ,ajouter le /
			html += "/";
		html += it->first;
		html += "\">";
		html += it->first;
		html += "</a></li>\n";
	}
	html += "</ul>\n</body>\n</html>\n";
	_body = html;
}

bool	Response::isAllowedMethod(const std::string &method, Route *route)
{
	if (!route)
		return false;

	const std::vector<std::string>& allowedMethods = route->getMethod();
	return (std::find(allowedMethods.begin(), allowedMethods.end(), method) != allowedMethods.end());
}

bool	Response::handleRequestTooLarge()
{
	if (_request->getBody().size() > (size_t)_config->getMaxBodySize())
	{
		buildErrorPage(413);
		_status = READY;
		return (true);
	}
	return (false);
}

bool	Response::handleUriTooLarge(const std::string &uri)
{
	if (uri.size() > MAX_URI_SIZE)
	{
		buildErrorPage(414);
		_status = READY;
		return (true);
	}
	return (false);
}

int		Response::isFileOrDir(const std::string &str)
{
	struct stat	buf;

	if (stat(str.c_str(), &buf) == -1)
		return (-1);
	if (S_ISREG(buf.st_mode))
		return (ISFILE);
	if (S_ISDIR(buf.st_mode))
		return (ISDIR);
	return (-1);
}

void	Response::interpretRequest()
{
	// Check si Cookie présent, sinon ajout au headers
	if (_request->getHeaders().find("Cookie") == _request->getHeaders().end())
		generateAndSetCookie();
	else
		_headers["Cookie"] = _request->getHeaders()["Cookie"];

	std::string method = _request->getMethod();
	std::string uri = _request->getUri();

	// Check le body size si trop grand, retourner 413
	// Check la size d'URI si trop grand, retourner 414
	if (handleUriTooLarge(uri) || handleRequestTooLarge())
		return (formatResponseToStr());

	// Check for route before expanding URI
	Route* route = _config->getRoute(uri);
	if (!route)
	{
		buildErrorPage(404);
		_status = READY;
		return (formatResponseToStr());
	}

	// Redirection si nécessaire
	if (route->getIsRedir())
	{
		_headers["Location"] = route->getRedirPath();
		buildErrorPage(301);
		_status = READY;
		return;
	}

	// Étendre l'URI avec l'index si nécessaire
	if (uri == "/")
	{
		if (_config->getIndex() != "")
			uri += _config->getIndex();
		// MAJ du request avec la nouvelle uri
		_request->setUri(uri);
	}

	std::string fullPath = _config->getRoot() + uri;

	// Check de version HTTP
	if (_request->getVersionHTTP() != HTTP_VERSION)
	{
		buildErrorPage(505);
		formatResponseToStr();
		_status = READY;
		return;
	}

	// Check si l'uri existe (bypass certains edge case de POST)
	if (isFileOrDir(fullPath) == -1)
	{
		buildErrorPage(404);
		_status = READY;
		formatResponseToStr();
		if (_headers["Content-Type"] == "text/html")
			_resultResponse = addFaviconToResponse(_resultResponse);
		return ;
	}

	// METHODES gérées: GET, POST et DELETE
	if (method == "POST" && isAllowedMethod(method, route) && !route->getCgiEnabled())
	{
		// Changer l'URI pour le mettre sur upload_file.py pour le POST, sans se preocupper de Cgi actif ou non
		uri = "/cgi-bin/upload_file.py";
		_request->setUri(uri);
		route = _config->getRoute(uri);
	}
	if (route->getCgi() && isCgiRequest(uri, route->getCgiExtension()) && isAllowedMethod(method, route))
	{
		// Test de CGI pour debug
		if (!_cgiHandler)
			_cgiHandler = new CgiHandler(route, _request, this, _config, _kqueue);
		if (_status != READY)
			_cgiHandler->handleCgi();
		if (_cgiHandler->getCgiOutputReady())
			_status = READY;
	}
	else if (method == "GET" && isAllowedMethod(method, route))
		handleGet(fullPath);
	else if (method == "DELETE" && isAllowedMethod(method, route))
		handleDelete(fullPath);
	else
	{
		buildErrorPage(405);
		_status = READY;
	}

	// Structurer la reponse en string a partir des datas de la classe pour envoi final
	if (_status == READY && _resultResponse.empty())
	{
		formatResponseToStr();
		// Ajout de l'icone favicon si le content-type est du html
		if (_headers["Content-Type"] == "text/html")
			_resultResponse = addFaviconToResponse(_resultResponse);
	}
}

void	Response::handleGet(const std::string &path)
{
	if (path == "www/server1/siege.txt")
	{
		_HTTPcode = 200;
		_statusMessage = "OK";
		_headers["Content-Type"] = "text/plain";
		_status = READY;
		return;
	}

	int fileOrDir = isFileOrDir(path);

	switch (fileOrDir)
	{
		case (ISFILE):
		{
			if (!_streamer)
			{
				_fileStream = new std::ifstream(path);
				_streamer = true;

				std::string extension = getFileExtension(path);
				_HTTPcode = 200;
				_headers["Content-Type"] = getContentType(extension);
				_statusMessage = "OK";
			}
			char buffer[CHUNK_SIZE];
			_fileStream->read(buffer, CHUNK_SIZE);
			std::streamsize bytesRead = _fileStream->gcount(); // Nombre de caractères réellement lus
			_body.append(buffer, bytesRead); // Ajouter uniquement les caractères lus au body

			if (_fileStream->eof())
			{
				_status = READY;
				_streamer = false;
				_fileStream->close();
				delete _fileStream;
				_fileStream = NULL;
			}
			else
				_status = BUILDING;

			return;
		}
		case (ISDIR):
		{
			if (_config->getAutoindex() == true)
			{
				buildListingPage();
				_HTTPcode = 200;
				_headers["Content-Type"] = "text/html";
				_statusMessage = "OK";
			}
			else
				buildErrorPage(403);
			_status = READY;
			return;
		}
		default:
		{
			buildErrorPage(404);
			_status = READY;
			return;
		}
	}
}

void	Response::handleDelete(const std::string &path)
{
	int	fileOrDir = isFileOrDir(path);

	switch (fileOrDir)
	{
		case (ISFILE):
		{
			if (remove(path.c_str()) == 0)
			{
				_body = "File successfully deleted";
				_HTTPcode = 200;
				_statusMessage = "OK";
			}
			else
			{
				_body = "Error deleting file";
				_HTTPcode = 500;
				_statusMessage = "Internal Server Error";
			}
			break;
		}
		case (ISDIR):
		{
			buildErrorPage(403);
			break;
		}
		default:
		{
			buildErrorPage(404);
			break;
		}
	}
	_status = READY;
}

void	Response::sendResponse()
{
	if (_totalBytes == 0) // First call, prepare the response string
	{
		formatResponseToStr();
		_totalBytes = _resultResponse.size();
		_bytesSent = 0; // Initialize bytes sent
	}

	size_t	remainingBytes = _totalBytes - _bytesSent;
	size_t	chunkSize = std::min(remainingBytes, (size_t)RESPONSE_SIZE);
	_responseChunk = _resultResponse.substr(_bytesSent, chunkSize);

	if (_request->getFD() == -1)
	{
		_status = SENT;
		return;
	}
	ssize_t bytesSent = send(_request->getFD(), _responseChunk.c_str(), chunkSize, 0);

	if (bytesSent > 0)
	{
		_bytesSent += bytesSent;
		if (_bytesSent >= _totalBytes)
			_status = SENT;
		else
			_status = WRITING;
	}
	else if (bytesSent == 0)
		_status = WRITING;
	else
		_status = SENT;
}

void	Response::generateAndSetCookie()
{
	// Generer une string aleatoire de COOKIE_SIZE caracteres pour la valeur du cookie
	std::string	chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string	cookieValue;
	int			size = COOKIE_SIZE;

	for (int i = 0; i < size; i++)
		cookieValue += chars[rand() % chars.size()];
	// Creer le cookie et l'ajouter aux headers avec le max-age COOKIE_AGE
	_cookie = "sessionId=" + cookieValue + "_cookie; Max-age=" + COOKIE_AGE;
	_headers["Set-Cookie"] = _cookie;
}
