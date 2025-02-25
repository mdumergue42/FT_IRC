/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 13:58:31 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/25 19:15:51 by basverdi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"
#include "includes/Libs.hpp"
#include <csignal>
#include <cstddef>
#include <stdexcept>
#include <sys/socket.h>

Server::Server(std::string port, std::string password) :
	_port(std::atoi(port.c_str())),
	_password(password),
	_run(true){
	_commandMap["PASS"] = &Server::handlePass;
	_commandMap["NICK"] = &Server::handleNick;
	_commandMap["USER"] = &Server::handleUser;
    _commandMap["JOIN"] = &Server::handleJoin;
	_commandMap["KICK"] = &Server::handleKick;
	_commandMap["INVITE"] = &Server::handleInvite;
	_commandMap["MODE"] = &Server::handleMode;
	_commandMap["TOPIC"] = &Server::handleTopic;
	_commandMap["PRIVMSG"] = &Server::handlePrivMsg;
	_commandMap["DIE"] = &Server::handleDie;
	_commandMap["QUIT"] = &Server::handleQuit;
}

Server::Server( Server const & copy ) {*this = copy;}

Server::~Server( void ) {
//	removeChannel();
	for (std::vector<Client *>::iterator it = _clientfds.begin(); it != _clientfds.end(); it++) {
		close((*it)->getFds());
		delete *it;
	}
}

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
		if (it->second == name && fd != it->first->getFds())
			return it->first;
	}
	return NULL;
}

//////// REMOVE ////////

void Server::removeClient(int fd) {
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		if (it->fd == fd) {
			_pollfds.erase(it);
			break;
		}
	}
	for (std::vector<Client*>::iterator it = _clientfds.begin(); it != _clientfds.end(); ++it) {
		if ((*it)->getFds() == fd) {
		//	delete *it; // avec pas de leaks mais invalid read size
			_clientfds.erase(it);
			break;
		}
	}
	close(fd);
	std::cout << YEL << "Client disconnected: " << fd << COLOR_RESET << std::endl;
}

void	Server::removeChannel(void) {
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		_channels.erase(it);
	}
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

std::string	Server::sendMess(int fds, std::string message) {
	message += "\n";
	const char *mess = message.c_str();
	send(fds, mess, strlen(mess), 0);
	std::cout << "Envoye : " + message;
	return message;
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

std::vector<std::string>	Server::targetSplit(std::string targets) {
	std::vector<std::string>	final;
	size_t	st = 0;
	size_t	end = targets.find(',');

	while (end != std::string::npos)
	{
		final.push_back(targets.substr(st, end - st));
		st = end + 1;
		end = targets.find(',', st);
	}
	final.push_back(targets.substr(st));
	return final;
}

void	Server::sendServerMessage(Client *client, std::string message) {
	for (size_t i = 0; i < _clientfds.size(); i++) {
		if (_clientfds[i] != client)
			send(_clientfds[i]->getFds(), message.c_str(), strlen(message.c_str()), 0);
	}
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
		throw std::runtime_error("Socket listening failed");

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

	std::cout << BLU <<"Nouveau client connect : " << client_socket << COLOR_RESET << std::endl;
        
    struct pollfd client_pollfd;
	client_pollfd.fd = client_socket;
	client_pollfd.events = POLLIN | POLLOUT;
	client_pollfd.revents = 0;
    _pollfds.push_back(client_pollfd);
}

void Server::processCommand(Client* client, int fd, const std::string &command) {
    try {
		std::vector<std::string> tokens = splitCom(command);
		if (tokens.empty() || tokens[0].empty())
		    return;
		std::string com = tokens[0];
		std::cout << "Recu : " + command << std::endl;
		if (!client->isAuth())
		{
		    if (command != "CAP LS 302\r" && com != "PASS")
		        throw	sendMess(fd, "You are not authenticated");
		} else if (client->getNickname().empty()) {
		    if (com != "NICK")
		        throw	sendMess(fd, "You must first define a nickname");
		} else if (client->getUsername().empty()) {
		    if (com != "USER")
		        throw	sendMess(fd, "You must first define a username");
		}
		if (command != "CAP LS 302\r") {
		    std::map<std::string, CommandFunc>::iterator it = _commandMap.find(com);
		    if (it != _commandMap.end()) {
		        CommandFunc func = it->second;
				(this->*func)(client, fd, tokens);
			}
		}
	}
    catch (const std::string &e) {}
}

void Server::run() {
	try {
	while (_run)
	{
		signal(SIGINT, signalHandler);
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
						_clientByN[client] = "";
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
							buffer.erase(0, pos + 1);
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
	catch(std::exception & e) {}
}




////////////////// COMMANDS ////////////////////////

void	Server::handlePass(Client* client, int fd, const std::vector<std::string>& tokens) {
	if (client->isAuth())
		throw	sendMess(fd, "462 " + client->getNickname() + ERR_ALREADYREGISTERED);
	if (tokens.size() < 2)
		throw	sendMess(fd, "461 " + client->getNickname() + tokens[0] + ERR_NEEDMOREPARAMS);
	if (tokens[1] != _password)
		throw	sendMess(fd, "464 " + client->getNickname() + ERR_PASSWDMISMATCH);
	client->setAuth(true);	
}


void	Server::handleNick(Client* client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw	sendMess(fd, "431 " + client->getNickname() + ERR_NONICKNAMEGIVEN);
	if (isTaken(1, tokens[1]))
        throw	sendMess(fd, ":localhost 433 " + client->getNickname() + " " + tokens[1] + ERR_NICKNAMEINUSE);
	std::string	oldNick = client->getNickname();
	client->setNickname(tokens[1]);
	_clientByN[client] = client->getNickname();
	if (client->getUsername().empty() && !client->isRegistered())
	{
		client->setRegister(true);
		sendMess(fd, ":localhost 001 " + client->getNickname() + " :Welcome to the IRC server\r\n");
	}
	else
	{
		std::string	mess;
		mess += ":" + oldNick + " NICK " + client->getNickname() + "\r\n";
		sendServerMessage(client, mess);
		sendMess(fd, mess.c_str());
	}
}

void	Server::handleUser(Client* client, int fd, const std::vector<std::string>& tokens) {
	std::string	oldUser = client->getUsername();
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("461 ") + client->getNickname() + " USER :Not enough parameters\r\n");
    if (isTaken(0, tokens[1])) {
        throw	sendMess(fd, codeErr("433") + tokens[1] + ERR_NICKNAMEINUSE);
	}
	if (oldUser.empty()) {
		sendMess(fd, codeErr("001") + client->getNickname() + " :Welcome to the ircserv Network, " + client->getNickname() + "[!" + tokens[1] + "@localhost]\r\n");
	}
	client->setRegister(true);
	client->setUsername(tokens[1]);
}

void	Server::handleJoin(Client * client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
	Channel* channel = getChannel(tokens[1]);
	if (!channel)
	{
		if (!channel->channelName(tokens[1]))
			throw	sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHCHANNEL);
		channel = new Channel(tokens[1]);
		channel->setOperator(client, false);
		client->setOp(true);
		_channels.push_back(channel);
	}
	if (channel->hasClient(client)) //discutable normalement on doit pas renvoyer d'erreurs mais c'est un peu golmon
		throw	sendMess(fd, codeErr("443") + client->getNickname() + " " + tokens[2] + " " + tokens[1] + ERR_USERONCHANNEL);
	if (!channel->getKey().empty())
		throw	sendMess(fd, codeErr("475") + client->getNickname() + " " + tokens[1] + ERR_BADCHANNELKEY);
	if (channel->isInviteOnly())
		throw	sendMess(fd, codeErr("473") + client->getNickname() + " " + tokens[1] + ERR_INVITEONLYCHAN);
	channel->addClient(client);
	std::string mess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost JOIN " + tokens[1] + "\r\n";
	channel->sendChannelMessage(client, mess);
	send(fd, mess.c_str(), strlen(mess.c_str()), 0);
	sendMess(fd, codeErr("332") + client->getNickname() + " " + tokens[1] + " :" + channel->getTopic());
	sendMess(fd, codeErr("353") + client->getNickname() + "=" + tokens[1] + " :@" + client->getNickname() + "\r\n");
	sendMess(fd, codeErr("366") + client->getNickname() + " " + tokens[1] + RPL_ENDOFNAMES);
}

void	Server::handleKick(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (!client->isOp())
		throw	sendMess(fd, codeErr("482") + client->getNickname() + " " + tokens[1] + ERR_CHANOPRIVSNEEDED);
	if (tokens.size() < 3)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
	Channel	*channel = getChannel(tokens[1]);
	if (!channel)
		throw	sendMess(fd, codeErr("403") + client->getNickname() + tokens[1] + ERR_NOSUCHCHANNEL);
	Client *target = getClientByNickname(fd, tokens[2]);
	if (target->getFds() == client->getFds())
		throw	sendMess(fd, ":localhost " + client->getNickname() + " :You can't kick yourself");
	if (!channel->hasClient(target))
		throw	sendMess(fd, codeErr("442") + target->getNickname() + " " + tokens[1] + ERR_NOTONCHANNEL);
	std::string	reason;
	if (!tokens[3].empty())
		reason += tokens[3];
	else
		reason += "No reason";
	std::string	mess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost KICK " + tokens[1] + target->getNickname() + reason + "\r\n";
	channel->sendChannelMessage(client, mess);
	send(fd, mess.c_str(), strlen(mess.c_str()), 0);
	sendMess(target->getFds(), "You have been kicked from " + tokens[1] + " by " + client->getNickname() + ". Reason: " + reason + "\r\n");
	channel->removeClient(target);
}

void	Server::handleInvite(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
	if (!client->isOp())
		throw	sendMess(fd, codeErr("482") + client->getNickname() + tokens[1] + ERR_CHANOPRIVSNEEDED);
	Channel	*channel = getChannel(tokens[1]);
	if (!channel)
		throw	sendMess(fd, codeErr("403") + client->getNickname() + tokens[1] + ERR_NOSUCHCHANNEL);
	if (!channel->isInviteOnly()) //discutable car ne doit pas renvoyer d'erreur juste ignorer
		throw	sendMess(fd, tokens[1] + " is not in invite mode.");
	Client *target = getClientByNickname(fd, tokens[2]);
	if (!target)
		throw	sendMess(fd, codeErr("401") + tokens[2] + ERR_NOSUCHNICK);
	if (channel->hasClient(target)) //discutable
		throw	sendMess(fd, codeErr("443") + client->getNickname() + " " + tokens[2] + " " + tokens[1] + ERR_USERONCHANNEL);
	if (target->getNickname().empty() || target->getUsername().empty())
		throw	sendMess(fd, codeErr("441") + target->getNickname() + tokens[1] + ERR_USERNOTINCHANNEL);
	std::string	mess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost INVITE " + target->getNickname() + tokens[1] + "\r\n";
	channel->sendChannelMessage(client, mess);
	sendMess(target->getFds(), "JOIN " + tokens[1] + "\r\n");
	sendMess(fd, codeErr("341") + client->getNickname() + " " + target->getNickname() + " " + tokens[1] + "\r\n");
	channel->addClient(target);
}

void	Server::handleTopic(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
	if (tokens.size() == 2)
	{
		Channel	*channel = getChannel(tokens[1]);
		if (!channel)
			throw	sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHCHANNEL);
		if (!channel->hasClient(client))
			throw	sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[1] + ERR_NOTONCHANNEL);
		if (channel->getTopic().empty())
			sendMess(fd, codeErr("331") + client->getNickname() + " " + tokens[1] + RPL_NOTOPIC);
		else if (!channel->getTopic().empty())
		{
			sendMess(fd, codeErr("332") + client->getNickname() + " " + tokens[1] + " :" + channel->getTopic() + "\r\n");
			sendMess(fd, codeErr("333") + client->getNickname() + " " + tokens[1] + " " + channel->getTopicWriter() + "\r\n");
		}
	}
	else
	{
		Channel	*channel = getChannel(tokens[1]);
		if (!channel)
			throw	sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHCHANNEL);
		if (!channel->hasClient(client))
			throw	sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[1] + ERR_NOTONCHANNEL);
		if (!client->isOp())
			throw	sendMess(fd, codeErr("482") + client->getNickname() + tokens[1] + ERR_CHANOPRIVSNEEDED);
		else
		{
			channel->setTopic(tokens[2]);
			channel->setTopicWriter(client->getNickname());
		}
		std::string	mess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost " + tokens[0] + " " + tokens[1] + " :" + tokens[2];
		channel->sendChannelMessage(client, mess);
		send(fd, mess.c_str(), strlen(mess.c_str()), 0);
	}
}

void	Server::handleMode(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 3)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);

	std::string channelName = tokens[1];
	Channel *channel = getChannel(channelName);
	if (!channel)
		throw	sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHCHANNEL);
	if (!channel->hasClient(client))
		throw	sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[1] + ERR_NOTONCHANNEL);
	if (!client->isOp())
		throw	sendMess(fd, codeErr("482") + client->getNickname() + tokens[1] + ERR_CHANOPRIVSNEEDED);
	
	std::string modeStr = tokens[2];
	bool add = true;
	int paramIndex = 3;

	for (size_t i = 0; i < modeStr.size(); i++) {
		char c = modeStr[i];
		if (c == '+')
			add = true;
		else if (c == '-')
			add = false;
		else {
			switch(c) {
				case 'i':
					channel->setInviteOnly(add);
					break;
				case 't':
					channel->setTopicRestricted(add);
					break;
				case 'k':
					if (add) {
                        if (tokens.size() <= (size_t)paramIndex) {
							throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
                            return;
                        }
                        channel->setKey(tokens[paramIndex]);
                        paramIndex++;
                    } else {
                        channel->setKey("");
                    }
                    break;
				case 'l':
					if (add) {
                        if (tokens.size() <= (size_t)paramIndex) //discutable car ca met juste pas de limite : chelou
						{
                            throw	sendMess(fd, "MODE: Missing parameter for limit");
                            return;
                        }
                        int limit = std::atoi(tokens[paramIndex].c_str());
                        channel->setUserLimit(limit);
                        paramIndex++;
                    } else {
                        channel->setUserLimit(0);
                    }
                    break;
				case 'o':
                    if (tokens.size() <= (size_t)paramIndex)
						throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
                    {
                        std::string targetNickname = tokens[paramIndex];
                        Client* target = getClientByNickname(fd, targetNickname);
						if (!target)
							throw	sendMess(fd, codeErr("401") + tokens[2] + ERR_NOSUCHNICK);
						if (channel->hasClient(target)) //discutable
							throw	sendMess(fd, codeErr("443") + client->getNickname() + " " + tokens[2] + " " + tokens[1] + ERR_USERONCHANNEL);
                        channel->setOperator(target, add);
                        paramIndex++;
                    }
                    break;
                default:
                    throw sendMess(fd, codeErr("472") + client->getNickname() + " " + tokens[2] + ERR_UNKNOWNMODE);
                    break;
			}
		}
	}
	
}

void	Server::handlePrivMsg(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("411") + client->getNickname() + ERR_NORECIPIENT + "(" + tokens[0] + ")");
	if (tokens.size() < 3)
		throw	sendMess(fd, codeErr("412") + client->getNickname() + ERR_NOTEXTTOSEND);
	std::string	fullMessage = "";
	std::vector<std::string> targets = targetSplit(tokens[1]);

	for (size_t i = 2; i < tokens.size(); i++) {
		if (i == tokens.size() - 1)
			fullMessage += tokens[i];
		else
			fullMessage += tokens[i] + " ";
	}
	fullMessage += "\r\n";
	for	(std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); it++) {
		std::string target = *it;
		Client *tmp = getClientByNickname(client->getFds(), *it);
		if (target[0] == '#')
		{
			Channel *channel = getChannel(*it);
			if (!channel)
				throw	sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHCHANNEL);
			if (!channel->hasClient(client))
				throw	sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[1] + ERR_NOTONCHANNEL);
			else
			{
				std::string	messChannel = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost PRIVMSG " + channel->getName() + " :" + fullMessage;
				channel->sendChannelMessage(client, messChannel);
			}
		}
		else
		{
			std::string	messClient = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost PRIVMSG " + tmp->getNickname() + " :" + fullMessage;
			send(tmp->getFds(), messClient.c_str(), strlen(messClient.c_str()), 0);
		}
	}
}

///////////////////////////////////WARNING///////////////////////
//////////gros probleme -> faire un quit puis un die avec un autre client 1 leak a cause du quit sauf que
///////////////////////////si je rajoute le delete qu'il faut c'est pire donc nsm

void	Server::handleDie(Client *client, int fd, const std::vector<std::string>& tokens) {
	(void)client;
	if (tokens.size() != 1)
		throw	sendMess(fd, "DIE: Too much parameter");
	_run = false;
//	for (size_t i = 0; i < _clientfds.size(); i++) {
//		removeClient(_clientfds[i]->getFds());
//	}
//	removeClient(fd);
//	removeChannel();
}

void	Server::handleQuit(Client *client, int fd, const std::vector<std::string>& tokens) {
	std::string	reason;
	if (tokens.size() < 2)
		reason += " :No reason";
	else
	{
		for (size_t i = 1; i < tokens.size(); ++i) {
			reason += " " + tokens[i];
		}
	}
	for (size_t i = 0; i < _channels.size(); i++) {
		if (_channels[i]->hasClient(client))
			_channels[i]->removeClient(client);
	}
	std::string fullMess = "hola cmt taler taler vou nsm";
//	std::string	fullMess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost QUIT" + reason + "\r\n";
	sendServerMessage(client, fullMess);
	removeClient(fd);
	delete client;
	close(fd);
}
