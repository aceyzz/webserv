#pragma once

#include "Webserv.hpp"

enum eSocketType
{
	CLIENT,
	SERVER
};

class	Socket
{
	private:
		eSocketType	_type;
		int			_fd;
		sockaddr_in	_addr;
		int			_port;
		std::string	_ip;
		int			_serverPort;

	public:
		Socket();
		Socket(eSocketType type, int port, std::string ip);
		Socket(eSocketType socktype, int domain, int type, int protocol, int port, std::string ip);
		Socket(eSocketType socktype, int domain, int port, std::string ip, int fd);
		~Socket();

		// Getters
		eSocketType	getType() const { return (_type); };
		int			getFD() const { return (_fd); };
		sockaddr_in	getAddr() const { return (_addr); };
		int			getPort() const { return (_port); };
		std::string	getIp() const { return (_ip); };
		int			getServerPort() const { return (_serverPort); };

		// Setters
		void	setType(eSocketType type) { _type = type; };
		void	setFD(int fd) { _fd = fd; };
		void	setAddr(sockaddr_in addr) { _addr = addr; };
		void	addPort(int port) { _port = port; };
		void	setIp(std::string ip) { _ip = ip; };
		void	setServerPort(int port) { _serverPort = port; };

		// Methods
		void	bindSocket();
		void	listenSocket();

		// Client
		int		acceptSocket(int eventfd);

		// Debug
		void	printSocket();
};
