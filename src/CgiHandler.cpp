#include "CgiHandler.hpp"

CgiHandler::CgiHandler(Route* route, Request* request, Response* response, Config* config, int kqueue)
{
	_kqueue = kqueue;
	_route = route;
	_request = request;
	_response = response;
	_config = config;
	_pipeFd[0] = -1;
	_pipeFd[1] = -1;
	_envp = NULL;
	_args = NULL;
	_responseLine = "";
	_cgiOutputResult = "";
	_cgiOutputReady = false;
	_bytesWritten = 0;
	_cgiLaunched = false;
	_cgiPid = -1;
	_uploadFilename = "";

	initArgs();
	initEnvp();
	initPipe();

	if (isMultipart())
		extractMultipartData();
}

void	CgiHandler::initEnvp()
{
	// Init des envs
	std::vector<std::string> envp;
	envp.push_back("SERVER_NAME=" + _config->getName());
	envp.push_back("SERVER_PROTOCOL=" + _request->getVersionHTTP());
	envp.push_back("SERVER_PORT=" + std::to_string(_config->getPort()));
	envp.push_back("REQUEST_METHOD=" + _request->getMethod());
	envp.push_back("REQUEST_URI=" + _request->getUri());
	envp.push_back("PATH_INFO=" + _request->getUri());
	envp.push_back("SCRIPT_NAME=" + _request->getUri());
	envp.push_back("QUERY_STRING=" + _request->getParam());
	envp.push_back("REMOTE_ADDR=" + _response->getClientSocket()->getIp());
	envp.push_back("REMOTE_PORT=" + std::to_string(_response->getClientSocket()->getPort()));
	envp.push_back("CONTENT_LENGTH=" + std::to_string(_request->getBody().size()));
	envp.push_back("CONTENT_TYPE=" + _request->getHeaders()["Content-Type"]);
	// Creation du tableau d'env
	_envp = new char*[envp.size() + 1];
	for (size_t i = 0; i < envp.size(); i++)
		_envp[i] = strdup(envp[i].c_str());
	_envp[envp.size()] = NULL;
}

void	CgiHandler::initArgs()
{
	// Init des args
	Cgi *cgi = _route->getCgi();
	std::vector<std::string> path = split(cgi->getPath(), " ");
	std::vector<std::string> extensions = split(cgi->getExtension(), " ");
	
	// Stocker chaque extension avec son path
	std::map<std::string, std::string> cgiPairs;
	for (size_t i = 0; i < extensions.size(); i++)
		cgiPairs[extensions[i]] = path[i];
	
	// Determiner la paire correspondante avec l'extension contenue dans l'uri
	std::string extension = getFileExtension(_request->getUri());
	std::string cgiPath = cgiPairs[extension];
	
	// Correction du chemin pour inclure le répertoire root du serveur
	std::string scriptPath = _config->getRoot() + _request->getUri();

	// Creation du tableau d'args
	_args = new char*[3];
	_args[0] = strdup(cgiPath.c_str());
	_args[1] = strdup(scriptPath.c_str());
	_args[2] = NULL;
}

void	CgiHandler::initPipe()
{
	if (pipe(_pipeFd) == -1)
	{
		std::cerr << REDD "[Warning] pipe() failed" RST << std::endl;
		_response->buildErrorPage(500);
		_response->setStatus(READY);
		_response->formatResponseToStr();
	}
	else
	{
		// Set le pipe en non bloquant
		if (fcntl(_pipeFd[0], F_SETFL, O_NONBLOCK) == -1)
		{
			close(_pipeFd[0]);
			close(_pipeFd[1]);
			_pipeFd[0] = -1;
			_pipeFd[1] = -1;
			std::cerr << REDD "[Warning] fcntl() failed" RST << std::endl;
			_response->buildErrorPage(500);
			_response->setStatus(READY);
			_response->formatResponseToStr();
		}
	}
	if (pipe(_pipeFdCgi) == -1)
	{
		std::cerr << REDD "[Warning] pipe() failed" RST << std::endl;
		_response->buildErrorPage(500);
		_response->setStatus(READY);
		_response->formatResponseToStr();
	}
	else
	{
		// Set le pipe en non bloquant
		if (fcntl(_pipeFdCgi[0], F_SETFL, O_NONBLOCK) == -1)
		{
			close(_pipeFdCgi[0]);
			close(_pipeFdCgi[1]);
			_pipeFdCgi[0] = -1;
			_pipeFdCgi[1] = -1;
			std::cerr << REDD "[Warning] fcntl() failed" RST << std::endl;
			_response->buildErrorPage(500);
			_response->setStatus(READY);
			_response->formatResponseToStr();
		}
	}
}

CgiHandler::~CgiHandler()
{
	if (_envp)
	{
		for (size_t i = 0; _envp[i]; i++)
			free(_envp[i]);
		delete[] _envp;
	}
	if (_args)
	{
		for (size_t i = 0; _args[i]; i++)
			free(_args[i]);
		delete[] _args;
	}
	if (_pipeFd[0] != -1)
		close(_pipeFd[0]);
	if (_pipeFd[1] != -1)
		close(_pipeFd[1]);
	if (_pipeFdCgi[0] != -1)
		close(_pipeFdCgi[0]);
	if (_pipeFdCgi[1] != -1)
		close(_pipeFdCgi[1]);
}

void	CgiHandler::printCgiHandler()
{
	std::cout << std::endl;
	std::cout << GOLD "CgiHandler:" RST << std::endl;
	std::cout << CYAN "  _kqueue: " RST << _kqueue << std::endl;
	std::cout << CYAN "  _route (address): " RST << _route << std::endl;
	std::cout << CYAN "  _request (address): " RST << _request << std::endl;
	std::cout << CYAN "  _response (address): " RST << _response << std::endl;
	std::cout << CYAN "  _config (address): " RST << _config << std::endl;
	std::cout << CYAN "  _pipeFd[0]: " RST << _pipeFd[0] << std::endl;
	std::cout << CYAN "  _pipeFd[1]: " RST << _pipeFd[1] << std::endl;
	std::cout << CYAN "  _pipeFdCgi[0]: " RST << _pipeFdCgi[0] << std::endl;
	std::cout << CYAN "  _pipeFdCgi[1]: " RST << _pipeFdCgi[1] << std::endl;
	std::cout << CYAN "  _envp (address): " RST << _envp << std::endl;
	for (size_t i = 0; _envp[i]; i++)
		std::cout << "    " << GRY1 "[" << i << "] -> " RST << _envp[i] << std::endl;
	std::cout << CYAN "  _args (address): " RST << _args << std::endl;
	for (size_t i = 0; _args[i]; i++)
		std::cout << "    " << GRY1 "[" << i << "] -> " RST << _args[i] << std::endl;
	std::cout << CYAN "  _responseLine: " RST << _responseLine << std::endl;
	std::cout << CYAN "  _cgiOutputResult: " RST << _cgiOutputResult << std::endl;
	std::cout << std::endl;
}

void	CgiHandler::handleCgi()
{
	if (DEBUG)
	{
		printCgiHandler();
		std::cout << CYAN "Body sent into the pipe: " RST << std::endl << _request->getBody() << std::endl;
	}

	_response->setStatus(BUILDING);

	// Si CGI n'est pas encore lancé, premier appel donc init + fork + execve
	if (!_cgiLaunched)
	{
		// Check si le body est vide, construire la page 400 et set la response comme ready
		if (_request->getBody().empty() && _request->getMethod() == "POST")
		{
			_response->buildErrorPage(400);
			_response->setStatus(READY);
			_response->formatResponseToStr();
			return;
		}

		// Si pas de content length, construire la page 411 et set la response comme ready
		if (_request->getHeaders().find("Content-Length") == _request->getHeaders().end())
		{
			_response->buildErrorPage(411);
			_response->setStatus(READY);
			_response->formatResponseToStr();
			return;
		}

		if (pipe(_pipeFd) == -1 || pipe(_pipeFdCgi) == -1)
		{
			std::cerr << GOLD "[Warning]" RST << " pipe() failed" RST << std::endl;
			_response->buildErrorPage(500);
			_response->setStatus(READY);
			_response->formatResponseToStr();
			return;
		}

		_cgiPid = fork();
		if (_cgiPid == -1)
		{
			std::cerr << GOLD "[Warning]" RST << " fork() failed" RST << std::endl;
			_response->buildErrorPage(500);
			_response->setStatus(READY);
			_response->formatResponseToStr();
			return;
		}

		if (_cgiPid == 0) // Enfant
		{
			close(_pipeFd[1]);
			dup2(_pipeFd[0], STDIN_FILENO);
			close(_pipeFd[0]);

			close(_pipeFdCgi[0]);
			dup2(_pipeFdCgi[1], STDOUT_FILENO);
			close(_pipeFdCgi[1]);

			execve(_args[0], _args, _envp);
			exit(EXIT_FAILURE);
		}
		else // Parent
		{
			close(_pipeFd[0]);
			close(_pipeFdCgi[1]);
			_cgiLaunched = true;
			_lastActivity = std::time(NULL);
		}
	}

	// Écriture par chunks de CHUNK_SIZE dans le pipe
	if (_bytesWritten < _request->getBody().size())
	{
		size_t chunkSize = std::min(static_cast<size_t>(CHUNK_SIZE), _request->getBody().size() - _bytesWritten);
		ssize_t written = write(_pipeFd[1], _request->getBody().c_str() + _bytesWritten, chunkSize);
		_lastActivity = std::time(NULL);

		// Afficher la progression si l'upload
		if (_request->getMethod() == "POST" && _request->getUri() == "/cgi-bin/upload_file.py")
		{
			double progress = static_cast<double>(_bytesWritten) / _request->getBody().size() * 100; // Calculate progress percentage
			progress = floor(progress * 100) / 100; // Limit to 2 decimal places
			double bytesWrittenMb = static_cast<double>(_bytesWritten) / 1e+6; // Convert bytes to megabytes
			bytesWrittenMb = floor(bytesWrittenMb * 100) / 100; // Limit to 2 decimal places
			std::cout << CURSOR "(" REDD << _uploadFilename << RST ") Mb uploaded: " GOLD << bytesWrittenMb << RST << "\tProgress: " CYAN << progress << "%" RST CURSOREND << std::endl;
		}

		if (written >= 0)
			_bytesWritten += written;
		else if (written == -1) 
		{
			std::cerr << GOLD "[Warning]" RST << " write() failed" RST << std::endl;
			_response->buildErrorPage(500);
			_response->setStatus(READY);
			close(_pipeFd[1]);
			return;
		}

		// Si tout le body a été écrit, fermer le descripteur d'écriture (envoyer EOF au CGI)
		if (_bytesWritten == _request->getBody().size())
			close(_pipeFd[1]);
	}

	_response->setStatus(BUILDING);

	// "Attendre" la fin du processus enfant avec l'option WNOHANG, pour serveur non bloquant
	int status;
	pid_t result = waitpid(_cgiPid, &status, WNOHANG);

	// Si le processus CGI est terminé et que le body a été entièrement écrit
	if (result <= -1)
	{
		std::cerr << GOLD "[Warning]" RST << " waitpid() failed" RST << std::endl;
		_response->buildErrorPage(500);
		_response->setStatus(READY);
		_response->formatResponseToStr();
		return;
	}
	else if (result > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0 && _bytesWritten == _request->getBody().size())
	{
		_lastActivity = std::time(NULL);
		// Lire la réponse complète du processus CGI
		while (true)
		{
			char buffer[BUFFER_SIZE];
			ssize_t bytesRead = read(_pipeFdCgi[0], buffer, BUFFER_SIZE - 1);
			if (bytesRead > 0)
			{
				buffer[bytesRead] = '\0';
				_cgiOutputResult += buffer;
			}
			else if (bytesRead == 0) // EOF
			{
				_cgiOutputReady = true;
				close(_pipeFdCgi[0]);
				break;
			}
			else
			{
				std::cerr << GOLD "[Warning]" RST << " read() failed" RST << std::endl;
				_response->buildErrorPage(500);
				_response->setStatus(READY);
				_response->formatResponseToStr();
				close(_pipeFdCgi[0]);
				return;
			}
		}

		// Si le CGI a tout écrit, la réponse est prête à être envoyée au client
		if (_cgiOutputReady)
		{
			std::cout << CLRALL << std::endl;
			_response->_headers["Content-Type"] = extractContentTypeCgiOutput();
			_response->_body = getCgiOutputResult();
			_response->_headers["Content-Length"] = std::to_string(_response->_body.size());
			
			// Si upload de fichier, retourner 201, sinon 200
			if (_request->getMethod() == "POST" && _request->getUri() == "/cgi-bin/upload_file.py")
			{
				_response->_HTTPcode = 201;
				_response->_statusMessage = "Created";
			}
			else
			{
				_response->_HTTPcode = 200;
				_response->_statusMessage = "OK";
			}
			_response->_status = READY;
			_response->formatResponseToStr();
			return ;
		}
	}
	else if (result == 0)
		_response->setStatus(BUILDING);
	else
	{
		std::cerr << GOLD "[Warning]" RST << " CGI process exited abnormally" RST << std::endl;
		close(_pipeFd[1]);
		close(_pipeFdCgi[0]);
		_response->buildErrorPage(502);
		_response->setStatus(READY);
		_response->formatResponseToStr();
		return ;
	}

	if (_lastActivity + CGI_TIMEOUT < std::time(NULL))
	{
		std::cerr << GOLD "[Warning]" RST << " CGI process timed out" RST << std::endl;
		kill(_cgiPid, SIGKILL);
		close(_pipeFd[1]);
		close(_pipeFdCgi[0]);
		_response->buildErrorPage(504);
		_response->setStatus(READY);
		_response->formatResponseToStr();
		return ;
	}
}

std::string	CgiHandler::extractContentTypeCgiOutput()
{
	std::string contentType = _cgiOutputResult.substr(0, _cgiOutputResult.find("\n"));
	_cgiOutputResult = _cgiOutputResult.substr(_cgiOutputResult.find("\n") + 1);
	return (contentType);
}

bool	CgiHandler::isMultipart()
{
	std::string contentType = _request->getHeaders()["Content-Type"];
	if (contentType.find("multipart/form-data") != std::string::npos)
		return (true);
	return (false);
}

void	CgiHandler::extractMultipartData()
{
	std::string contentType = _request->getHeaders()["Content-Type"];
	size_t boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos)
	{
		std::cerr << GOLD "Boundary non trouvé dans Content-Type." RST << std::endl;
		return;
	}

	std::string boundary = "--" + contentType.substr(boundaryPos + 9);
	std::string body = _request->getBody();

	// Recherche du premier occurrence du boundary
	size_t partStart = body.find(boundary);
	if (partStart == std::string::npos)
	{
		std::cerr << GOLD "Boundary initial non trouvé dans le corps." RST << std::endl;
		return;
	}
	partStart += boundary.length(); // Juste après le boundary

	// Gestion des fins de ligne juste après le boundary
	while (body.substr(partStart, 1) == "\n" || body.substr(partStart, 1) == "\r")
		partStart += 1;

	// Recherche de la fin des en-têtes
	size_t headersEnd = body.find("\r\n\r\n", partStart);
	if (headersEnd == std::string::npos)
	{
		headersEnd = body.find("\n\n", partStart);
		if (headersEnd == std::string::npos)
		{
			std::cerr << GOLD "Fin des en-têtes non trouvée." RST << std::endl;
			return;
		}
		else
			headersEnd += 2; // Sauter '\n\n'
	}
	else
		headersEnd += 4; // Sauter '\r\n\r\n'

	size_t fileStart = headersEnd;
	size_t fileEnd = body.find(boundary, fileStart) - 1; // Trouver le prochain boundary et s'arrêter juste avant
	if (body[fileEnd - 1] == '\r')
		fileEnd -= 1; // Si CRLF, reculer un caractère de plus

	std::string fileContent = body.substr(fileStart, fileEnd - fileStart);

	// Extraction du nom du fichier
	std::string headers = body.substr(partStart, headersEnd - partStart);
	size_t filenamePos = headers.find("filename=\"");
	std::string filename = "unknown";
	if (filenamePos != std::string::npos)
	{
		size_t filenameStart = filenamePos + 10;
		size_t filenameEnd = headers.find("\"", filenameStart);
		if (filenameEnd != std::string::npos)
			filename = headers.substr(filenameStart, filenameEnd - filenameStart);
	}

	_request->setBody(fileContent);  // Réassignation du corps de la requête avec le contenu binaire

	// Ajout du filename dans les variables d'environnement
	std::vector<std::string> newEnvs;
	for (size_t i = 0; _envp[i]; i++)
		newEnvs.push_back(_envp[i]);
	newEnvs.push_back("FILENAME=" + filename);
	_uploadFilename = filename;
	for (size_t i = 0; _envp[i]; i++)
		free(_envp[i]);
	delete[] _envp;
	_envp = new char*[newEnvs.size() + 1];
	for (size_t i = 0; i < newEnvs.size(); i++)
		_envp[i] = strdup(newEnvs[i].c_str());
	_envp[newEnvs.size()] = NULL;
}
