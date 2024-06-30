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
	Cgi	*cgi = _route->getCgi();
	std::vector<std::string>	path = split(cgi->getPath(), " ");
	std::vector<std::string>	extensions = split(cgi->getExtension(), " ");
	// Stocker chaque extension avec son path
	std::map<std::string, std::string>	cgiPairs;
	for (size_t i = 0; i < extensions.size(); i++)
		cgiPairs[extensions[i]] = path[i];
	// Determiner la paire correspondante avec l'extension contenue dans l'uri
	std::string	extension = getFileExtension(_request->getUri());
	std::string	cgiPath = cgiPairs[extension];
	// Creation du tableau d'args
	_args = new char*[3];
	_args[0] = strdup(cgiPairs[extension].c_str());
	_args[1] = strdup(_request->getUri().c_str());
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
	std::cout << GRY1 "CgiHandler destroyed" RST << std::endl;
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
