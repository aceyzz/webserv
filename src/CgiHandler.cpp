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

	initArgs();
	initEnvp();
	initPipe();
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
		std::cerr << REDD "Error: pipe() failed" RST << std::endl;
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
			std::cerr << REDD "Error: fcntl() failed" RST << std::endl;
			_response->buildErrorPage(500);
			_response->setStatus(READY);
			_response->formatResponseToStr();
		}
	}
	if (pipe(_pipeFdCgi) == -1)
	{
		std::cerr << REDD "Error: pipe() failed" RST << std::endl;
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
			std::cerr << REDD "Error: fcntl() failed" RST << std::endl;
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

void CgiHandler::handleCgi()
{
	std::string body = _request->getBody();
	std::ofstream logFile("log/request_to_cgi.log", std::ios_base::app);
	logFile << body << std::endl;
	logFile.close();

	if (DEBUG)
	{
		printCgiHandler();
		std::cout << CYAN "Body sent into the pipe: " RST << std::endl << _request->getBody() << std::endl;
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		std::cerr << "Error: fork() failed" << std::endl;
		_response->buildErrorPage(500);
		_response->setStatus(READY);
		_response->formatResponseToStr();
		return;
	}
	if (pid == 0) // Enfant
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

		// Écriture complète du body de la requête dans le pipe
		ssize_t written = write(_pipeFd[1], _request->getBody().c_str(), _request->getBody().size());
		if (written == -1)
		{
			std::cerr << "Error: write() failed" << std::endl;
			_response->buildErrorPage(500);
			_response->setStatus(READY);
			_response->formatResponseToStr();
			return;
		}
		close(_pipeFd[1]); // Fermer le descripteur d'écriture après avoir tout envoyé

		// Attendre la fin du processus enfant
		int status;
		waitpid(pid, &status, 0);

		// Lire la réponse complète du processus CGI
		char buffer[BUFFER_SIZE];
		ssize_t bytesRead;
		while ((bytesRead = read(_pipeFdCgi[0], buffer, BUFFER_SIZE - 1)) > 0)
		{
			buffer[bytesRead] = '\0';
			_cgiOutputResult += buffer;
		}
		if (bytesRead == -1 && errno != EAGAIN)
		{
			std::cerr << "Error: read() failed" << std::endl;
			_response->buildErrorPage(500);
			_response->setStatus(READY);
			_response->formatResponseToStr();
			return;
		}
		if (bytesRead == 0)
		{
			_cgiOutputReady = true;
			close(_pipeFdCgi[0]);
		}

		if (_cgiOutputReady)
		{
			_response->_headers["Content-Type"] = extractContentTypeCgiOutput();
			_response->_body = getCgiOutputResult();
			_response->_headers["Content-Length"] = std::to_string(_response->_body.size());
			_response->_HTTPcode = 200;
			_response->_statusMessage = "OK";
			_response->_status = READY;
		}
	}
}

std::string	CgiHandler::extractContentTypeCgiOutput()
{
	std::string contentType = _cgiOutputResult.substr(0, _cgiOutputResult.find("\n"));
	_cgiOutputResult = _cgiOutputResult.substr(_cgiOutputResult.find("\n") + 1);
	return (contentType);
}
