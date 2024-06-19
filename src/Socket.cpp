#include "Socket.hpp"

Socket::Socket()
{
	_fd = -1;
	_port = -1;
	_ip = "";
}

Socket::Socket(eSocketType type, int port, std::string ip)
{
	_type = type;
	_fd = -1;
	_port = port;
	_ip = ip;
}

Socket::Socket(eSocketType socktype, int domain, int type, int protocol, int port, std::string ip)
{
	// Creation du socket
	_type = socktype;
	_fd = socket(domain, type, protocol);
	if (_fd == -1)
		throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
	_port = port;
	_ip = ip;

	// Creation de l'adresse
	_addr.sin_family = domain;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

Socket::Socket(eSocketType socktype, int domain, int port, std::string ip, int fd)
{
	_type = socktype;
	_fd = fd;
	_port = port;
	_ip = ip;

	// Creation de l'adresse
	_addr.sin_family = domain;
	_addr.sin_port = htons(port);
	_addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

Socket::~Socket()
{
	if (_fd != -1)
	{
		close(_fd);
		_fd = -1;
	}
}

void	Socket::bindSocket()
{
	if (bind(_fd, (sockaddr*)&_addr, sizeof(_addr)) == -1)
		throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
}

void	Socket::listenSocket()
{
	if (listen(_fd, MAX_LISTEN) == -1)
		throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
}

int Socket::acceptSocket(int eventFD)
{
	sockaddr_in	clientAddr;
	socklen_t	clientAddrLen = sizeof(clientAddr);
	int			clientFD = accept(eventFD, (sockaddr*)&clientAddr, &clientAddrLen);

	if (clientFD == -1)
	{
		std::cerr << "accept() failed: " << strerror(errno) << std::endl;
		return (-1);
	}

	if (fcntl(clientFD, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl() failed: " << strerror(errno) << std::endl;
		close(clientFD);
		return (-1);
	}

	return (clientFD);
}

void	Socket::printSocket()
{
	std::cout << std::endl;
	std::cout << GOLD "- SOCKET -" << std::endl;
	std::cout << CYAN "  Type: " RST << (_type == 1 ? "Server" : "Client") << std::endl;
	std::cout << CYAN "  FD: " RST << _fd << std::endl;
	std::cout << CYAN "  Port: " RST << _port << std::endl;
	std::cout << CYAN "  IP: " RST << _ip << std::endl;
	std::cout << std::endl;
}
