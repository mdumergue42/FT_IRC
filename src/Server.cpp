/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 13:58:31 by madumerg          #+#    #+#             */
/*   Updated: 2025/03/07 18:04:48 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"
#include "includes/Libs.hpp"
#include <csignal>
#include <sstream>
#include <cstddef>
#include <stdexcept>
#include <sys/socket.h>

volatile std::sig_atomic_t	run_signal = 1;

void	signalHandler(int signum) {
	if (signum == SIGINT)
		run_signal = 0;
}

Server::Server(std::string port, std::string password) :
	_port(std::atoi(port.c_str())),
	_password(password){
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

	std::cout << "🚀 Initialisation des commandes serveur... ✅" << std::endl;
}

Server::Server( Server const & copy ) {*this = copy;}

Server::~Server( void ) {
	close(_server_fd);

	for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); it++) {
		close(it->fd);
	}
	_pollfds.clear();
	for (std::vector<Client *>::iterator it = _clientfds.begin(); it != _clientfds.end(); it++) {
		close((*it)->getFds());
		delete *it;
	}
	_clientfds.clear();
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		delete *it;
	}
	_channels.clear();
	_clientByN.clear();
	std::cout << "🧹 [CLEANUP] Fermeture du serveur, nettoyage des clients et canaux..." << std::endl;
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

std::string to_string_compat(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string Server::getDisplayName(int fd) {
    Client* client = getFdsClient(fd);
    if (client && !client->getNickname().empty())
        return to_string_compat(fd) + ":" + client->getNickname();
    return to_string_compat(fd);
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
			delete *it;
			_clientfds.erase(it);
			break;
		}
	}
	close(fd);
	std::cout << YEL << "👋 [DISCONNECT] Client déconnecté: " << getDisplayName(fd) << RESET << std::endl;
}

void	Server::removeChannel(void) {
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
		_channels.erase(it);
	}
	std::cout << YEL << "🏷️ [CHANNELS] Suppression de tous les canaux..." << RESET << std::endl;
}

///////////// Utils ///////////////

bool	Server::isValidNickname(std::string nickname) {
	if (!(nickname.size() > 0 && nickname.size() < 10))
		return false;
	const char	arrErr[25] = {'@', '!', '#', '$', '%', '^', '&', '*', '(', ')', '=', '+', '{', '}', '[', ']', ':', ';', '"', '\'', '<', '>', ',', '?', '/'};
	const char	arrF[12] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'};
	const char	arrV[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',                           
    '-', '_',                                                               
    '\0'                                                                       
};

	for (size_t i = 0; i < nickname.size(); i++) {
		for (size_t j = 0; j < 25; j++) {
			if (nickname[i] == arrErr[j])
				return false;
		}
	}
	for (size_t i = 0; i < 12; i++) {
		if (nickname[0] == arrF[i])
			return false;
	}

	for (size_t i = 0; i < nickname.size(); i++) {
		bool valid = false;
		for (size_t j = 0; arrV[j] != '\0'; j++) {
			if (nickname[i] == arrV[j]) {
				valid = true;
				break;
			}
		}
		if (!valid)
			return false;
	}

	return true;
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

std::string	Server::sendMess(int fds, std::string message) {
	ssize_t sent = send(fds, message.c_str(), message.size(), 0);
	if (sent == -1) {
		std::cerr << RED << "❌ [ERROR] Envoi échoué au client " << fds << " : " << strerror(errno) << RESET << std::endl;
		return "";
	}
	std::cout << CYN << "📤 [SEND] [->" << getDisplayName(fds) << "] " << message << RESET;
	return message;
}


std::vector<std::string> Server::splitCommas(const std::string &input) {
	std::vector<std::string> result;
	std::string token;
	std::istringstream tokenStream(input);
	while (std::getline(tokenStream, token, ',')) {
		result.push_back(token);
	}
	return result;
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
	try {
	this->_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_server_fd == -1)
		throw	std::runtime_error("Server socket creation failed");
	int opt = 1;
	if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
	
	std::cout << "🧱 Création du socket serveur... ✅" << std::endl;
	
	int flags = fcntl(_server_fd, F_GETFL, 0);
	fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK);

	std::cout << "⚡️ Passage du socket en mode non-bloquant... ✅" << std::endl;

	sockaddr_in	adServ;
	adServ.sin_family = AF_INET;
	adServ.sin_port = htons(this->_port);
	adServ.sin_addr.s_addr = INADDR_ANY;

	std::cout << "📍 Configuration de l’adresse et du port... (port " << this->_port << ")" << std::endl;

	if (bind(this->_server_fd, (struct sockaddr *)&adServ, sizeof(adServ)) < 0)
		throw std::runtime_error("Socket binding failed");
	
	std::cout << "🔗 Liaison du socket à l’adresse et au port... ✅" << std::endl;
	
	if (listen(this->_server_fd, 5) > 0)
		throw std::runtime_error("Socket listening failed");
	
	std::cout << "👂 Mise en écoute sur le port " << this->_port << "... ✅" << std::endl;
	
	pollfd server_pollfd;
	memset(&server_pollfd, 0, sizeof(server_pollfd));
	server_pollfd.fd = this->_server_fd;
	server_pollfd.events = POLLIN;
	_pollfds.push_back(server_pollfd);
	
	std::cout << "🚀 Serveur prêt et en attente de connexions ! 🎉" << std::endl;
	}
	catch(const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
}

void Server::NewClient() {
	struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(_pollfds[0].fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_socket < 0)
		throw std::runtime_error("Client connection failed");

	std::cout << GRN << "🆕 [CONNECT] Client connecté sur fd " << client_socket << RESET << std::endl;        
    
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
		std::cout << MAG << "📥 [COMMAND] [<-" << getDisplayName(fd) << "] " << command << WHT << std::endl;
		if (!client->isAuth()) {
			if (command != "CAP LS 302\r" && com != "PASS")
				throw sendMess(fd, ":localhost 451 " + client->getNickname() + " :You have not registered\r\n");
    	} else if (client->getNickname().empty() || client->getNickname() == "*") {
			if (com != "NICK")
		        throw sendMess(fd, ":localhost 431 * :No nickname given\r\n");
		} else if (client->getUsername().empty()) {
		    if (com != "USER")
		        throw sendMess(fd, ":localhost 451 " + client->getNickname() + " :You have not registered\r\n");
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
		std::cout << "🏃 Lancement de la loop principale du serveur..." << std::endl;
		while (run_signal) {
			signal(SIGINT, signalHandler);
			int poll_count = poll(_pollfds.data(), _pollfds.size(), -1);
			if (poll_count > 0) {
				if (_pollfds[0].revents & POLLIN)
					NewClient();
				for (size_t i = _pollfds.size() - 1; i > 0; i--) {
					if (!run_signal)
						break;
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
								if (!run_signal)
									break;
							}
						}
						else if (bytes == 0) {
							removeClient(_pollfds[i].fd);
							break;
						}
						else {
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
		throw	sendMess(fd, codeErr("462") + client->getNickname() + ERR_ALREADYREGISTERED);
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " PASS" + ERR_NEEDMOREPARAMS);
	if (tokens[1] != _password)
		throw	sendMess(fd, codeErr("464") + client->getNickname() + ERR_PASSWDMISMATCH);
	client->setAuth(true);
	std::cout << GRN << "✅ [AUTH] [" << getDisplayName(fd) << "] is Authenticated" << RESET << std::endl;
}

void	Server::handleNick(Client* client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("431") + client->getNickname() + ERR_NONICKNAMEGIVEN);
	if (isValidNickname(tokens[1]) == false)
		throw	sendMess(fd, codeErr("432") + "client " + tokens[1] + ERR_ERRONEUSNICKNAME);
	if (isTaken(1, tokens[1]))
        throw	sendMess(fd, codeErr("433") + client->getNickname() + " " + tokens[1] + ERR_NICKNAMEINUSE);
	std::string	oldNick = client->getNickname();
	client->setNickname(tokens[1]);
	_clientByN[client] = client->getNickname();
	if (client->getUsername().empty() && !client->isRegistered()) {
		client->setRegister(true);
		std::cout << GRN << "✅ [NICK] [" << fd << ":" + oldNick + "] is now registered as " << client->getNickname() << RESET << std::endl;
	} else {
		std::string	mess;
		mess += ":" + oldNick + " NICK " + client->getNickname() + "\r\n";
		sendServerMessage(client, mess);
		sendMess(fd, mess.c_str());
	}
}

void	Server::handleUser(Client* client, int fd, const std::vector<std::string>& tokens) {
	std::string	oldUser = client->getUsername();
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " USER" + ERR_NEEDMOREPARAMS);
    if (isTaken(0, tokens[1])) {
        throw	sendMess(fd, codeErr("433") + tokens[1] + ERR_NICKNAMEINUSE);
	}
	if (oldUser.empty()) {
		time_t now = time(NULL);
		std::string creationTime = ctime(&now);
		creationTime = creationTime.substr(0, creationTime.size() - 1);
		sendMess(fd, codeErr("001") + client->getNickname() + RPL_WELCOME + client->getNickname() + "[!" + tokens[1] + "@localhost]\r\n");	
		sendMess(fd, codeErr("002") + client->getNickname() + RPL_YOURHOST);
		sendMess(fd, codeErr("003") + client->getNickname() + RPL_CREATED + creationTime + "\r\n");
		sendMess(fd, codeErr("004") + client->getNickname() + RPL_MYINFO);
		sendMess(fd, codeErr("005") + client->getNickname() + RPL_ISUPPORT);
	}
	client->setRegister(true);
	client->setUsername(tokens[1]);
}

void	Server::handleJoin(Client * client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " JOIN" + ERR_NEEDMOREPARAMS);
	
	std::vector<std::string> channelList = splitCommas(tokens[1]);

	std::vector<std::string> keyList;
	if (tokens.size() >= 3)
		keyList = splitCommas(tokens[2]);
	
	for (size_t i = 0; i < channelList.size(); i++) {
		std::string chanName = channelList[i];
		if (chanName.empty() || (chanName[0] != '#' && chanName[0] != '&'))
			throw sendMess(fd, codeErr("403") + client->getNickname() + " " + chanName + ERR_NOSUCHCHANNEL);

		std::string key = (i < keyList.size() ? keyList[i] : "");
		
		Channel* channel = getChannel(chanName);
		if (!channel) {
			channel = new Channel(chanName);
			channel->setOperator(client, true);
			_channels.push_back(channel);
		}

		if (channel->hasClient(client))
			throw sendMess(fd, codeErr("443") + client->getNickname() + " " + chanName + ERR_USERONCHANNEL);

		if (!channel->getKey().empty() && channel->getKey() != key)
			throw sendMess(fd, codeErr("475") + client->getNickname() + " " + chanName + ERR_BADCHANNELKEY);

		if (channel->isInviteOnly() && channel->isInInviteList(client) == false)
			throw sendMess(fd, codeErr("473") + client->getNickname() + " " + chanName + ERR_INVITEONLYCHAN);

		if (channel->getUserLimit() > 0 && channel->getClients().size() == channel->getUserLimit())
			throw sendMess(fd, codeErr("471") + client->getNickname() + " " + chanName + ERR_CHANNELISFULL);
		
		channel->addInviteList(client, false);
		channel->addClient(client);
	
		std::string mess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost JOIN " + chanName + "\r\n";
		channel->sendChannelMessage(client, mess);
		send(fd, mess.c_str(), strlen(mess.c_str()), 0);
		if (!channel->getTopic().empty())
			sendMess(fd, codeErr("332") + client->getNickname() + " " + chanName + " :" + channel->getTopic() + "\r\n");
		std::string names;
		std::vector<Client*> members = channel->getClients();
		for (size_t j = 0; j < members.size(); j++) {
			Client* member = members[j];
			bool op = channel->isOperator(member);
			if (op)
				names += "@" + member->getNickname() + " ";
			else
				names += member->getNickname() + " ";
		}
		sendMess(fd, codeErr("353") + client->getNickname() + " = " + chanName + " :" + names + "\r\n");
		sendMess(fd, codeErr("366") + client->getNickname() + " " + chanName + RPL_ENDOFNAMES);
	}
}

void	Server::handleKick(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 3)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " KICK" + ERR_NEEDMOREPARAMS);
	Channel	*channel = getChannel(tokens[1]);
	if (!channel)
		throw	sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHCHANNEL);
	if (channel->hasClient(client) == false)
		throw	sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[1] + ERR_NOTONCHANNEL);
	if (channel->isOperator(client) == false)
		throw	sendMess(fd, codeErr("482") + client->getNickname() + " " + tokens[1] + ERR_CHANOPRIVSNEEDED);
	Client *target = getClientByNickname(fd, tokens[2]);
	if (!target)
		throw	sendMess(fd, codeErr("401") + client->getNickname() + " " + tokens[2] + ERR_NOSUCHNICK);
	if (channel->hasClient(target) == false)
		throw	sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[2] + ERR_NOTONCHANNEL);
	std::string	reason = (tokens.size() > 3) ? tokens[3] : "No reason";
	std::string	mess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost KICK " + tokens[1] + " " + target->getNickname() + " :" + reason + "\r\n";
	channel->sendChannelMessage(client, mess);
	send(fd, mess.c_str(), strlen(mess.c_str()), 0);
	send(target->getFds(), mess.c_str(), strlen(mess.c_str()), 0);	
	channel->removeClient(target);
}

void	Server::handleInvite(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 3)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " INVITE" + ERR_NEEDMOREPARAMS);

	Client *target = getClientByNickname(fd, tokens[1]);
	if (!target)
		throw	sendMess(fd, codeErr("401") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHNICK);

	Channel	*channel = getChannel(tokens[2]);
	if (!channel)
		throw	sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[2] + ERR_NOSUCHCHANNEL);
	if (channel->hasClient(client) == false)
		throw	sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[2] + ERR_NOTONCHANNEL);
	if (channel->isOperator(client) == false)
		throw	sendMess(fd, codeErr("482") + client->getNickname() + " " + tokens[2] + ERR_CHANOPRIVSNEEDED);
	if (channel->hasClient(target))
		throw	sendMess(fd, codeErr("443") + client->getNickname() + " " + tokens[1] + " " + tokens[2] + ERR_USERONCHANNEL);

	std::string	mess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost INVITE " + target->getNickname() + tokens[2] + "\r\n";
	std::string targetmess = "You have been invited to " + tokens[2] + " by " + client->getNickname() + " (localhost)\r\n";
	sendMess(target->getFds(), targetmess);
	channel->sendChannelMessage(client, mess);
	channel->addInviteList(target, true);
	sendMess(fd, codeErr("341") + client->getNickname() + " " + target->getNickname() + " " + tokens[2] + "\r\n");
}

void	Server::handleTopic(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
	
	Channel	*channel = getChannel(tokens[1]);
	
	if (!channel)
		throw sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHCHANNEL);
	
	if (channel->hasClient(client) == false)
		throw sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[1] + ERR_NOTONCHANNEL);
	
	if (tokens.size() == 2) {
		if (channel->getTopic().empty())
			sendMess(fd, codeErr("331") + client->getNickname() + " " + tokens[1] + RPL_NOTOPIC);
		else {
			sendMess(fd, codeErr("332") + client->getNickname() + " " + tokens[1] + " :" + channel->getTopic() + "\r\n");
			sendMess(fd, codeErr("333") + client->getNickname() + " " + tokens[1] + " " + channel->getTopicWriter() + " " + channel->getTopicTime() + "\r\n");
		}
		return;
	}
	
	if (channel->isTopicRestricted() && channel->isOperator(client) == false)
		throw sendMess(fd, codeErr("482") + client->getNickname() + " " + tokens[1] + ERR_CHANOPRIVSNEEDED);
	
	std::string newTopic;
	for (size_t i = 2; i < tokens.size(); i++) {
		if (i == 2 && tokens[i][0] == ':')
			newTopic += tokens[i].substr(1);
		else
			newTopic += tokens[i];
		if (i != tokens.size() - 1)
			newTopic += " ";
	}
	
	channel->setTopic(newTopic);
	channel->setTopicWriter(client->getNickname());
	channel->setTopicTime(time(NULL));

	std::string	mess = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost TOPIC " + tokens[1] + " :" + newTopic + "\r\n";
	channel->sendChannelMessage(client, mess);
	send(fd, mess.c_str(), strlen(mess.c_str()), 0);
}

void	Server::handleMode(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() == 2) {
		std::string channelName = tokens[1];
		Channel *channel = getChannel(channelName);
		if (!channel)
			throw sendMess(fd, codeErr("403") + client->getNickname() + " " + channelName + ERR_NOSUCHCHANNEL);

		std::string currentModes;
		if (channel->isOperator(client) == false)
			currentModes = channel->getModes();
		else	
			currentModes = channel->getModesOps();
		sendMess(fd, codeErr("324") + client->getNickname() + " " + channelName + " " + currentModes + "\r\n");
		return;
	}

	if (tokens.size() < 3)
		throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);

	std::string channelName = tokens[1];
	Channel *channel = getChannel(channelName);
	if (!channel)
		throw	sendMess(fd, codeErr("403") + client->getNickname() + " " + tokens[1] + ERR_NOSUCHCHANNEL);
	if (channel->hasClient(client) == false)
		throw	sendMess(fd, codeErr("442") + client->getNickname() + " " + tokens[1] + ERR_NOTONCHANNEL);
	if (channel->isOperator(client) == false)
		throw	sendMess(fd, codeErr("482") + client->getNickname() + " " + tokens[1] + ERR_CHANOPRIVSNEEDED);
	
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
                        if (tokens.size() <= (size_t)paramIndex)
							throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
                        channel->setKey(tokens[paramIndex]);
                        paramIndex++;
                    } else {
                        channel->setKey("");
                    }
                    break;
				case 'l':
					if (add) {
                        if (tokens.size() <= (size_t)paramIndex)
                            throw	sendMess(fd, codeErr("461") + client->getNickname() + " " + tokens[0] + ERR_NEEDMOREPARAMS);
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
						if (channel->hasClient(target) == false)
							throw	sendMess(fd, codeErr("441") + client->getNickname() + " " + tokens[2] + " " + tokens[1] + ERR_USERNOTINCHANNEL);
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
	std::string extraParams;
	for (size_t i = 3; i < tokens.size(); i++) {
		extraParams += " " + tokens[i];
	}
    std::string modeMessage = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost MODE " + channelName + " " + modeStr + extraParams + "\r\n";
	send(fd, modeMessage.c_str(), strlen(modeMessage.c_str()), 0);
    channel->sendChannelMessage(client, modeMessage);
}

void	Server::handlePrivMsg(Client *client, int fd, const std::vector<std::string>& tokens) {
	if (tokens.size() < 2)
		throw	sendMess(fd, codeErr("411") + client->getNickname() + ERR_NORECIPIENT + " (PRIVMSG)\r\n");
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
			if (channel->hasClient(client) == false)
				throw	sendMess(fd, codeErr("404") + client->getNickname() + " " + tokens[1] + ERR_CANNOTSENDTOCHAN);
			else
			{
				std::string	messChannel = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost PRIVMSG " + channel->getName() + " " + fullMessage;
				channel->sendChannelMessage(client, messChannel);
			}
		}
		else
		{
			if (!tmp)
				throw	sendMess(fd, codeErr("401") + client->getNickname() + " " + tokens[2] + ERR_NOSUCHNICK);
			std::string	messClient = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost PRIVMSG " + tmp->getNickname() + " :" + fullMessage;
			send(tmp->getFds(), messClient.c_str(), strlen(messClient.c_str()), 0);
			std::cout << BLU << "📤 [SEND] [->" << tmp->getNickname() << "] " << messClient << RESET;
		}
	}
}

void	Server::handleDie(Client *client, int fd, const std::vector<std::string>& tokens) {	
	(void)client;
	(void)tokens;
	std::string name = getDisplayName(fd);
	std::cout << RED << "🔥 [ALERTE] DIE reçue de " << name << ". Extinction du serveur." << RESET << std::endl;

	run_signal = 0;	
	std::cout << GRN << "✅ Tous les clients et canaux ont été déconnectés." << RESET << std::endl;
	std::cout << RED << "☠️  [SHUTDOWN] Serveur éteint par " << name << RESET << std::endl;
}
