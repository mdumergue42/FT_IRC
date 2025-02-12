/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 13:58:31 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/12 16:09:52 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"

Server::Server(std::string port, std::string password) :
	_port(std::atoi(port.c_str())),
	_password(password) {
	_commandMap["PASS"] = &Server::handlePass;
	_commandMap["NICK"] = &Server::handleNick;
	_commandMap["USER"] = &Server::handleUser;
    _commandMap["JOIN"] = &Server::handleJoin;
	_commandMap["KICK"] = &Server::handleKick;
}

Server::Server( Server const & copy ) {*this = copy;}

Server::~Server( void ) {}

Server &	Server::operator=( Server const & op ) {
	if (this != &op)
		return *this;
	return *this;
}


////////////// Get ////////////////

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

Client*	Server::getClientByNickname(int fd, std::string name) {
	std::map<Client *, std::string>::iterator it;
	for (it = _clientByN.begin() ; it != _clientByN.end(); it++) {
		std::cout << "it s-> " << it->second << "| name -> " << name << std::endl;
		if (it->second == name && fd != it->first->getFds())
			return it->first;
	}
	return NULL;
}


///////////// Utils ///////////////

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
	message += "\n";
	const char *mess = message.c_str();
	send(fds, mess, strlen(mess), 0);
	std::cout << message;
}

std::vector<std::string> Server::splitCom(const std::string &input) {
    std::vector<std::string> commands;
    std::string tmp = "";

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


//////////// Exec //////////////

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

	std::cout << "\033[32mNouveau client connectÃ© : " << client_socket << "\033[0m" << std::endl;
        
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
    if (command == "DATA") {
        std::cout << "\033[36m=== Données des Clients sur le serveur ===\033[0m" << std::endl;
        for (size_t i = 0; i < _clientfds.size(); i++) {
            Client* client = _clientfds[i];
            std::cout << "Client FD: " << client->getFds() << std::endl;
            std::cout << "Nickname: " << client->getNickname() << std::endl;
            std::cout << "Username: " << client->getUsername() << std::endl;
            std::cout << "Authenticated: " << (client->isAuth() ? "Yes" : "No") << std::endl;
            std::cout << "Buffer: " << client->getBuffer() << std::endl;
			std::cout << "Channels: ";
            bool inChannel = false;
          /*  for (size_t j = 0; j < _channels.size(); j++) {
                if (_channels[j]->hasClient(client)) {  // Vérifier si le client est dans ce channel
                    std::cout << _channels[j]->getName() << " ";
                    inChannel = true;
                }
            }*/
            if (!inChannel)
                std::cout << "None";
            std::cout << "-------------------------------------" << std::endl;
        }
        return;
    }
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
        std::map<std::string, CommandFunc>::iterator it = _commandMap.find(com);
        if (it != _commandMap.end()) {
            CommandFunc func = it->second;
            (this->*func)(client, fd, tokens);
        } else {
            sendErrMess(fd, "Unknown command: " + com);
        }
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
					bytes = recv(_pollfds[i].fd, temp, sizeof(temp) - 1, 0);
					if (bytes > 0) {
						temp[bytes]= '\0';
						buffer += temp;

						size_t pos;
						while ((pos = buffer.find("\n")) != std::string::npos) {
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


////////////////// COMMANDS ////////////////////////

void	Server::handlePass(Client* client, int fd, const std::vector<std::string>& tokens) {
	if (client->isAuth()) {
		sendErrMess(fd, "You are already authenticated");
		return ;
	}
	if (tokens.size() < 2) {
		sendErrMess(fd, "PASS: Missing parameter");
	}
	if (tokens[1] != _password) {
		sendErrMess(fd, "Password incorrect");
		return ;
	}
	client->setAuth(true);
	sendErrMess(fd, "You was authenticated");
}

void	Server::handleNick(Client* client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2) {
        sendErrMess(fd, "NICK: Missing parameter");
        return ;
    }
    if (isTaken(1, tokens[1]))
        sendErrMess(fd, "Nickname is already used");
    else {
        client->setNickname(tokens[1]);
		_clientByN[client] = client->getNickname();
        sendErrMess(fd, "Nickname accepted");
    }
}

void	Server::handleUser(Client* client, int fd, const std::vector<std::string>& tokens) {
	if (client->getNickname().empty()) {
        sendErrMess(fd, "Define a nickname first using NICK.");
        return ;
    }
    if (tokens.size() < 2) {
        sendErrMess(fd, "USER: Missing parameter.");
        return; }
    if (isTaken(0, tokens[1]))
        sendErrMess(fd, "Username is already used.");
    else {
        client->setUsername(tokens[1]);
		client->setAuth(true);
        sendErrMess(fd, "User accepted and authenticated.");
    }
}

void	Server::handleJoin(Client * client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2) {
		sendErrMess(fd, "JOIN: Missing parameter.");
		return; }
	if (!client->isAuth()) {
		sendErrMess(fd, "You are not authenticated");
		return; }
	Channel* channel = getChannel(tokens[1]);
	if (!channel)
	{
		channel = new Channel(tokens[1]);
		client->setOp(true);
		
		_channels.push_back(channel);
		_clientByN[client] = "";
	}
	channel->addClient(client);
	sendErrMess(fd, "Joined channel " + tokens[1]);
}


void	Server::handleKick(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (!client->isAuth()) {
		sendErrMess(fd, "You are not authenticated.");
		return; }
	if (!client->isOp()) {
		sendErrMess(fd, "You are not operator.");
		return; }
	if (tokens.size() < 2) {
		sendErrMess(fd, "KICK: Missing parameter.");
		return; }
	Client *target = getClientByNickname(fd, tokens[1]);
	if (target == NULL) {
		sendErrMess(fd, "The user you want to eject does not exist or you can't kill yourself");
		return; }
	Channel	*channel = NULL;
	channel->removeClient(target);
}

/*void	Server::handleInvite(Client *client, int fd, std::vector<std::string>& tokens) {

}*/
