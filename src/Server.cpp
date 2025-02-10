/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 13:58:31 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/10 23:17:05 by bastienverdie    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"
#include <stdexcept>

Server::Server( void ) {}

Server::Server(std::string port, std::string password) :
	_port(std::atoi(port.c_str())),
	_password(password) {}

Server::Server( Server const & copy ) {*this = copy;}

Server::~Server( void ) {}

Server &	Server::operator=( Server const & op ) {
	if (this != &op)
		return *this;
	return *this;
}

Client *	Server::getFdsClient( int fds ) {
	for (size_t i = 0; i < _clientfds.size(); i++) {
		if (_clientfds[i]->getFds() == fds)
			return _clientfds[i];
	}
	return NULL;
}


bool	Server::isTaken( int code, std::string name ) {
	bool	isTaken = false;
	if (code == 1)
	{
		for (size_t i = 0; i < _clientfds.size(); i++) {
			if (_clientfds[i]->getNickname() == name)
			{
				isTaken = true;
				break ;
			}
		}
		return isTaken;
	}
	else
	{
		for (size_t i = 0; i < _clientfds.size(); i++) {
			if (_clientfds[i]->getUsername() == name)
			{
				isTaken = true;
				break ;
			}
		}
		return isTaken;
	}
}

void	Server::sendErrMess(int & fds, std::string message ) {
	const char *mess = message.c_str();
	send(fds, mess, strlen(mess), 0);
	std::cout << message << std::endl;
}

std::vector<std::string>	Server::splitCom( char *buffer ) {
	std::vector<std::string>	commands;
	std::string	tmp = "";

	for (size_t i = 0; buffer[i] != '\0'; i++) {
		if (buffer[i] == ' ' || buffer[i] == '\n')
		{
			if (!tmp.empty())
			{
				commands.push_back(tmp);
				tmp = "";
			}
		}
		else
			tmp += buffer[i];
	}
	return commands;
}

void Server::initserv() {
	this->_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_server_fd == -1)
		throw	std::runtime_error("Server socket creation failed");
	sockaddr_in	adServ;

	adServ.sin_family = AF_INET;
	adServ.sin_port = htons(this->_port);
	adServ.sin_addr.s_addr = INADDR_ANY;

	if (bind(this->_server_fd, (struct sockaddr *)&adServ, sizeof(adServ)) < 0)
		throw std::runtime_error("Socket binding failed");

	if (listen(this->_server_fd, 5) > 0)
		throw std::runtime_error("non plus");

	pollfd server_pollfd;
	memset(&server_pollfd, 0, sizeof(server_pollfd));
	server_pollfd.fd = this->_server_fd;
	server_pollfd.events = POLLIN;
	_pollfds.push_back(server_pollfd);
}

void Server::NewClient() {
	struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(_pollfds[0].fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket < 0)
		throw std::runtime_error("Client connection failed");

	std::cout << "\033[32mNouveau client connecté : " << client_socket << "\033[0m" << std::endl;
        
    struct pollfd client_pollfd;
	client_pollfd.fd = client_socket;
	client_pollfd.events = POLLIN | POLLOUT;
    _pollfds.push_back(client_pollfd);
}

void Server::run() {
	while (1)
	{
		int poll_count = poll(_pollfds.data(), _pollfds.size(), -1);
		if (poll_count > 0)
		{
			if (_pollfds[0].revents & POLLIN)
				NewClient();

			for (size_t i = _pollfds.size() - 1; i > 0; i--) {
				if (_pollfds[i].revents & POLLIN) {
					char buffer[1024] = {0};
					ssize_t bytes = recv(_pollfds[i].fd, buffer, sizeof(buffer), 0);
					std::cout << bytes << std::endl;
					if (bytes <= 0)
					{
						std::cout << "\033[33mClient disconnected : " << _pollfds[i].fd << "\033[0m" << std::endl;
						close(_pollfds[i].fd);
			            _pollfds.erase(_pollfds.begin() + i);
					}
					else
					{
						std::cout << "Message du client : " << buffer;
						Client *client = getFdsClient(_pollfds[i].fd); //verifier si il existe pas deja
						if (!client)
						{
							client = new Client(_pollfds[i].fd);
							_clientfds.push_back(client);
						}

						std::vector<std::string> commands = this->splitCom(buffer); //pour recup par ex NICK clientyoupi
						std::string	com = commands[0];
						if (com == "NICK" && commands.size() > 1)
						{
							bool	isTaken = this->isTaken(1, commands[1]);
							if (isTaken)
								this->sendErrMess(_pollfds[i].fd, "Nickname is already used");
							else
							{
								client->setNickname(commands[1]);
								this->sendErrMess(_pollfds[i].fd, "Nickname accepted");
							}
						}
						else if (com == "USER" && commands.size() > 1)
						{
							std::cout << "inside USER\n";
							if (client->getNickname().empty())
								this->sendErrMess(_pollfds[i].fd, "You must first define a nickname. Ex : NICK yourname");
							bool	isTaken = this->isTaken(0, commands[1]);
							if (isTaken)
								this->sendErrMess(_pollfds[i].fd, "Username is already used");
							else
							{
								client->setUsername(commands[1]);
								client->setAuth(true);
								this->sendErrMess(_pollfds[i].fd, "Username accepted");
							}
						}
						else if (com == "JOIN")
						{
							if (!client->isAuth())
								this->sendErrMess(_pollfds[i].fd, "You are not authenticated");
							//chiant a venir
						}
						else
							std::cout << "tamere irc\n";
					}
				}
			}
		}
	}
	close(_server_fd);
}
