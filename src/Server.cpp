/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 13:58:31 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/12 03:13:14 by bastienverdie    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"

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

Channel* Server::getChannel(const std::string &channelName) {
    for (size_t i = 0; i < _channels.size(); i++) {
        if (_channels[i]->getName() == channelName)
            return _channels[i];
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


std::vector<std::string> Server::splitCom(const std::string &input) {
    std::vector<std::string> commands;
    std::string tmp = "";

    std::cout << "buf -> " << input << std::endl;
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == ' ' || input[i] == '\n' || input[i] == '\r') {
            if (!tmp.empty()) {
                commands.push_back(tmp);
                tmp = "";
            }
        }
        else {
            tmp += input[i];
        }
    }
    if (!tmp.empty())
        commands.push_back(tmp);
    return commands;
}


void Server::initserv() {
	this->_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_server_fd == -1)
		throw	std::runtime_error("Server socket creation failed");
	sockaddr_in	adServ;
	
	int flags = fcntl(_server_fd, F_GETFL, 0);
	fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK);

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

void Server::processCommand(Client* client, int fd, const std::string &command) {
    std::vector<std::string> tokens = splitCom(command);
    if (tokens.empty() || tokens[0].empty())
        return;
    std::string com = tokens[0];
	if (!client->isAuth())
	{
		if (command != "CAP LS 302\r" && com != "PASS") {
			sendErrMess(fd, "You are not authenticated");
			return;
		}
	} else if (client->getNickname().empty()) {
		if (com != "NICK" && com != "USER") {
			sendErrMess(fd, "You must first define a nickname or a username");
			return;
		}
	}
	if (command != "CAP LS 302\r") {
		//exec command
	}
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
					char temp[1024];
					ssize_t bytes;
					Client* client = getFdsClient(_pollfds[i].fd);
					if (!client) {
						client = new Client(_pollfds[i].fd);
						_clientfds.push_back(client);
					}
					std::string &buffer = client->getBuffer();
					while (1) {
						bytes = recv(_pollfds[i].fd, temp, sizeof(temp) - 1, 0);
						if (bytes > 0) {
							temp[bytes]= '\0';
							buffer += temp;

							size_t pos;
							while ((pos = buffer.find("\r\n")) != std::string::npos) {
								std::string command = buffer.substr(0, pos);
								buffer.erase(0, pos + 2);
								processCommand(client, _pollfds[i].fd, command);
							}
						}
						else if (bytes == 0)
						{
							removeClient(_pollfds[i].fd);
							break;
						}
						else
						{
							if (errno == EWOULDBLOCK || errno == EAGAIN)
								break;
							else {
								std::cerr << "Error receiving data on fd " << _pollfds[i].fd << std::endl;
								break;
							}
						}
					}
				}
			}
		}
	}
	close(_server_fd);
}


void Server::removeClient(int fd) {
    for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
        if (it->fd == fd) {
            _pollfds.erase(it);
            break;
        }
    }
    for (std::vector<Client*>::iterator it = _clientfds.begin(); it != _clientfds.end(); ++it) {
        if ((*it)->getFds() == fd) {
            delete *it;
            _clientfds.erase(it);
            break;
        }
    }
	close(fd);
	std::cout << "\033[33mClient disconnected: " << fd << "\033[0m" << std::endl;
}
