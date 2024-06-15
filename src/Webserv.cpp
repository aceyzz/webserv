#include "Webserv.hpp"

Webserver::~Webserver()
{
	for (size_t i = 0; i < _serverSockets.size(); i++)
		if (_serverSockets[i] != NULL)
			delete _serverSockets[i];
	for (size_t i = 0; i < _clientSockets.size(); i++)
		if (_clientSockets[i] != NULL)
			delete _clientSockets[i];
	for (std::map<int, Request*>::iterator it = _requests.begin(); it != _requests.end(); it++)
		if (it->second != NULL)
			delete it->second;
	if (_kqueue != -1)
		close(_kqueue);
}

void	Webserver::printConfigs()
{
	for (size_t j = 0; j < _configs.size(); j++)
	{
		Config* config = _configs[j];
		std::cout << std::endl;
		std::cout << CYAN << "PRINT CONFIG (CLASS WEBSERVER):" << RST << std::endl;
		std::cout << GRY2 "Address of config: " << config << std::endl;
		std::cout << YLLW << "Name: " << RST << config->getName() << std::endl;
		std::cout << YLLW << "Port: " << RST << config->getPort() << std::endl;
		std::cout << YLLW << "Root: " << RST << config->getRoot() << std::endl;
		std::cout << YLLW << "Index: " << RST << config->getIndex() << std::endl;
		std::cout << YLLW << "Error pages: " << RST << std::endl;
		std::map<int, std::string> errorPages = config->getErrorPages();
		for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); it++)
			std::cout << "  " << it->first << " -> " << it->second << std::endl;
		std::cout << YLLW << "Autoindex: " << RST << (config->getAutoindex() ? "true" : "false") << std::endl;
		std::cout << YLLW << "Max body size: " << RST << config->getMaxBodySize() << std::endl;
		std::cout << YLLW << "Routes: " << RST << std::endl;
		std::vector<Route*> routes = config->getRoutes();
		for (size_t i = 0; i < routes.size(); i++)
		{
			Route* route = routes[i];
			std::cout << "  " << GRY1 "Route [" << i+1 << "] " RST << std::endl;
			std::cout << "    " << YLLW << "Root: " << RST << route->getRoot() << std::endl;
			std::cout << "    " << YLLW << "Uri: " << RST << route->getUri() << std::endl;
			std::cout << "    " << YLLW << "Methods: " << RST << std::endl;
			std::vector<std::string> methods = route->getMethod();
			for (size_t k = 0; k < methods.size(); k++)
				std::cout << "      " << methods[k] << std::endl;
			std::cout << "    " << YLLW << "Index: " << RST << route->getIndex() << std::endl;
			std::cout << "    " << YLLW << "Cgi enabled: " << RST << (route->getCgiEnabled() ? "true" : "false") << std::endl;
			if (route->getCgiEnabled())
			{
				std::cout << "    " << YLLW << "Cgi path: " << RST << route->getCgiPath() << std::endl;
				std::cout << "    " << YLLW << "Cgi extension: " << RST << route->getCgiExtension() << std::endl;
			}
			std::cout << "    " << YLLW << "Is redir: " << RST << (route->getIsRedir() ? "true" : "false") << std::endl;
			if (route->getIsRedir())
				std::cout << "    " << YLLW << "Redir path: " << RST << route->getRedirPath() << std::endl;
		}
		std::cout << std::endl;
	}
}

void	Webserver::printSockets()
{
	std::cout << GOLD "PRINT SERVER SOCKETS (CLASS WEBSERVER):" RST << std::endl;
	for (size_t i = 0; i < _serverSockets.size(); i++)
	{
		Socket* socket = _serverSockets[i];
		socket->printSocket();
	}
}

bool	Webserver::isServerSocket(int fd)
{
	return (_serverSockets.find(fd) != _serverSockets.end());
}

void	Webserver::addServerSocket(Socket *socket)
{
	_serverSockets[socket->getFD()] = socket;
}

void	Webserver::addClientSocket(Socket *socket)
{
	_clientSockets[socket->getFD()] = socket;
}

void	Webserver::initServer()
{
	_kqueue = kqueue();
	if (_kqueue == -1)
		throw std::runtime_error("kqueue() failed");
	
	// Creation des sockets serveurs
	for (size_t i = 0; i < _configs.size(); i++)
	{
		Config* config = _configs[i];
		Socket* socket = new Socket(SERVER, AF_INET, SOCK_STREAM, 0, config->getPort(), config->getName());
		_serverSockets.insert(std::pair<int, Socket*>(socket->getFD(), socket));

		if (fcntl(socket->getFD(), F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));

		struct kevent	event;
		EV_SET(&event, socket->getFD(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(_kqueue, &event, 1, NULL, 0, NULL) == -1)
			throw std::runtime_error("kevent() failed: " + std::string(strerror(errno)));
	}

	// Bind et listen des sockets serveurs
	for (std::map<int, Socket*>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); ++it)
	{
		Socket* socket = it->second;
		socket->bindSocket();
		socket->listenSocket();
	}
}

void	Webserver::runServer()
{
	while (g_signal)
	{
		std::vector<struct kevent> events(MAX_EVENTS);
		int nbEvents = kevent(_kqueue, NULL, 0, &events[0], MAX_EVENTS, NULL);
		if (nbEvents == -1 && g_signal)
			throw std::runtime_error("kevent() failed: " + std::string(strerror(errno)));
		
		for (int i = 0; i < nbEvents; i++)
		{
			// Si EV_ERROR (erreur dans kevent) alors on continue
			if (events[i].flags & EV_ERROR)
			{
				std::cerr << "Error in kevent" << std::endl;
				continue;
			}

			// Si c'est un socket serveur
			if (isServerSocket(events[i].ident))
			{
				if (events[i].filter == EVFILT_READ)
					acceptNewClient(events[i].ident);
			}
			else
			{
				if (events[i].filter == EVFILT_READ)
				{
					if (receiveRequest(events[i].ident))
						parseAndHandleRequest(events[i].ident);
				}
				else if (events[i].filter == EVFILT_WRITE)
				{
				    // if (sendResponse(events[i].ident))
				        closeClient(events[i].ident);
				}
			}
		}
	}
}

bool	Webserver::receiveRequest(int clientFD)
{
	char	buffer[1024];
	ssize_t	nbBytes = recv(clientFD, buffer, sizeof(buffer) - 1, 0);

	if (nbBytes <= 0)
	{
		// Error handling or connection closed by client
		close(clientFD);
		_requests.erase(clientFD);
		_clientSockets.erase(clientFD);
		return (false);
	}

	buffer[nbBytes] = '\0';
	Request*	request = _requests[clientFD];
	if (!request)
	{
		// Find the client socket based on clientFD
		Socket*	clientSocket = _clientSockets[clientFD];
		if (!clientSocket)
		{
			std::cerr << "Client socket not found for FD: " << clientFD << std::endl;
			return (false);
		}

		request = new Request(clientFD, clientSocket->getIp());
		_requests[clientFD] = request;
	}

	request->appendRawRequest(buffer);

	if (nbBytes == 0 || static_cast<size_t>(nbBytes) < sizeof(buffer) - 1)
	{
		request->setStatus(COMPLETE);
		return (true);
	}

	if (DEBUG)
		std::cout << "Received " << nbBytes << " bytes from client: " << request->getClientIp() << " (FD: " << clientFD << ")" << std::endl;

	return (false);
}

void	Webserver::acceptNewClient(int serverFD)
{
	sockaddr_in	clientAddr;
	socklen_t	clientAddrLen = sizeof(clientAddr);

	// On accepte la connexion du client
	int			clientFD = accept(serverFD, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientFD == -1)
	{
		std::cerr << "accept() failed: " << strerror(errno) << std::endl;
		return;
	}
	// On met le client en mode non-bloquant
	if (fcntl(clientFD, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl() failed: " << strerror(errno) << std::endl;
		close(clientFD);
		return;
	}
	// On ajoute le client au kqueue
	struct kevent event;
	EV_SET(&event, clientFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(_kqueue, &event, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << "kevent() failed: " << strerror(errno) << std::endl;
		close(clientFD);
		return;
	}
	// On ajoute le client à la liste des sockets clients
	Socket* socket = new Socket(CLIENT, AF_INET, clientFD, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
	_clientSockets[clientFD] = socket;

	if (DEBUG)
		std::cout << "New client connected: " << socket->getIp() << ":" << socket->getPort() << std::endl;
}

void	Webserver::parseAndHandleRequest(int fd)
{
	Request* request = _requests[fd];

	if (!request)
	{
		std::cerr << "Request not found" << std::endl;
		close(fd);
		_requests.erase(fd);
		return ;
	}

	// Parser la requete et init des valeurs de la classe Request
	request->parseRequest(request->getRawRequest());

	if (DEBUG)
		request->printRequest();

	// Mise a jour de l'evenement kqueue pour l'ecriture
	struct kevent	event;
	EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(_kqueue, &event, 1, NULL, 0, NULL) == -1)
	{
		std::cerr << "kevent() failed: " << strerror(errno) << std::endl;
		close(fd);
		_requests.erase(fd);
		return ;
	}
}

void	Webserver::closeClient(int fd)
{
	if (fd != -1)
		close(fd);
	_requests.erase(fd);
	_clientSockets.erase(fd);

	if (DEBUG)
		std::cout << "Client closed: " << fd << std::endl;
}
