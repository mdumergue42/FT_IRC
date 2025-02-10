/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 13:58:31 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/10 03:13:49 by bastienverdie    ###   ########.fr       */
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

			for (size_t i = 1; i < _pollfds.size(); i++) {
				if (_pollfds[i].revents & POLLIN) {
					char buffer[1024] = {0};
					ssize_t bytes = recv(_pollfds[i].fd, buffer, sizeof(buffer), 0);
					std::cout << bytes << std::endl;
					if (bytes <= 0)
					{
						std::cout << "\033[33mClient disconnected : " << _pollfds[i].fd << "\033[0m" << std::endl;
						close(_pollfds[i].fd);
			            _pollfds.erase(_pollfds.begin() + i);
					} else 
						std::cout << "Message du client : " << buffer;
				}
			}
		}
	}
	close(_server_fd);
}
