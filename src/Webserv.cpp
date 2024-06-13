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
		_serverSockets.push_back(socket);

		if (fcntl(socket->getFD(), F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));

		struct kevent	event;
		EV_SET(&event, socket->getFD(), EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(_kqueue, &event, 1, NULL, 0, NULL) == -1)
			throw std::runtime_error("kevent() failed: " + std::string(strerror(errno)));
	}

	// Bind et listen des sockets serveurs
	for (size_t i = 0; i < _serverSockets.size(); i++)
	{
		Socket* socket = _serverSockets[i];
		socket->bindSocket();
		socket->listenSocket();
	}
}

void	Webserver::runServer()
{
	std::vector<struct kevent> events(MAX_EVENTS);
	int	nbRequest = 0;
	int	end = 2;

	while (g_signal && (nbRequest < end))
	{
		int nbEvents = kevent(_kqueue, NULL, 0, events.data(), MAX_EVENTS, NULL);
		if (nbEvents == -1 && g_signal)
			throw std::runtime_error("kevent() failed: " + std::string(strerror(errno)));

		nbRequest++;

		for (int i = 0; i < nbEvents; i++)
		{
			int eventFD = events[i].ident;
			int eventFilter = events[i].filter;

			if (events[i].flags & EV_ERROR)
			{
				std::cerr << "Error in kevent: " << strerror(events[i].data) << std::endl;
				continue;
			}

			bool isServerSocket = false;

			for (std::vector<Socket*>::iterator it = _serverSockets.begin(); it != _serverSockets.end(); it++)
			{
				Socket* socket = *it;
				if (eventFD == socket->getFD() && eventFilter == EVFILT_READ)
				{
					int clientFD = socket->acceptSocket(eventFD);
					if (clientFD == -1)
						throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
					Socket* clientSocket = new Socket(CLIENT, 0, socket->getPort(), socket->getIp(), clientFD);
					_clientSockets.push_back(clientSocket);

					struct kevent event;
					EV_SET(&event, clientFD, EVFILT_READ, EV_ADD, 0, 0, NULL);
					if (kevent(_kqueue, &event, 1, NULL, 0, NULL) == -1)
						throw std::runtime_error("kevent() failed: " + std::string(strerror(errno)));

					clientSocket->printSocket();
					isServerSocket = true;
					break;
				}
			}

			if (!isServerSocket)
			{
				if (eventFilter == EVFILT_READ)
					handleRequest(eventFD);
				else if (eventFilter == EVFILT_WRITE)
					std::cout << "Write event" << std::endl;
			}
		}
	}
}

void	Webserver::handleRequest(int fd)
{
	char buffer[1024];
	std::string rawRequest;
	int bytesRead;

	// Récupérer la requête existante ou en créer une nouvelle
	Request* request = getRequest(fd);
	if (request == nullptr)
	{
		Socket* clientSocket = nullptr;
		for (std::vector<Socket*>::iterator it = _clientSockets.begin(); it != _clientSockets.end(); it++)
		{
			if ((*it)->getFD() == fd)
			{
				clientSocket = *it;
				break;
			}
		}

		if (clientSocket == nullptr)
		{
			std::cerr << "Client socket not found" << std::endl;
			return;
		}

		std::string clientIp = clientSocket->getIp();
		request = new Request(fd, clientIp);
		_requests[fd] = request;
	}

	// Lire les données en fragments
	while ((bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
		buffer[bytesRead] = '\0';
		rawRequest.append(buffer);
	}

	if (bytesRead == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
	{
		std::cerr << "Error in recv: " << strerror(errno) << std::endl;
		return;
	}

	request->parseRequest(rawRequest);
	
	if (DEBUG)
		request->printRequest();
}
