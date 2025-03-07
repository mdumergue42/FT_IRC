/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 19:43:52 by madumerg          #+#    #+#             */
/*   Updated: 2025/03/05 11:14:01 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Channel.hpp"
#include "includes/Libs.hpp"
#include <climits>
#include <string>

Channel::Channel( void ) {}

Channel::Channel(std::string name) :
	_name(name),
	_topic(""),
	_inviteOnly(false),
	_topicRestricted(false),
	_key(""),
	_userLimit(0) {}

Channel::Channel( Channel const & copy ) {*this = copy;}

Channel::~Channel( void ) {}

Channel & Channel::operator=( Channel const & op ) {
	if (this != &op)
		return *this;
	return *this;
}

std::string Channel::getName() const {return _name;}
const std::vector<Client*>& Channel::getClients() const {return _clients;}
std::string	Channel::getTopic() const {return _topic;}
std::string	Channel::getTopicWriter() const {return _topicWriter;}
std::string Channel::getTopicTime() const {
    std::ostringstream oss;
    oss << _topicTime;
    return oss.str(); 
}
std::string Channel::getKey() const {return _key;}
long unsigned int Channel::getUserLimit() const {return _userLimit;}
std::string	Channel::getModes() const {
	std::string	modeStr;
	std::string params;

	if (_inviteOnly)
		modeStr.push_back('i');
	if (_topicRestricted)
		modeStr.push_back('t');
	if (!_key.empty())
		modeStr.push_back('k');
	if (_userLimit > 0) {
		modeStr.push_back('l');
		std::ostringstream oss;
        oss << _userLimit;
		params += " " + oss.str();
	}
	if (modeStr.empty())
		return "";
	return "+" + modeStr + params;
}
std::string	Channel::getModesOps() const {
	std::string	modeStr;
	std::string params;

	if (_inviteOnly)
		modeStr.push_back('i');
	if (_topicRestricted)
		modeStr.push_back('t');
	if (!_key.empty()) {
		modeStr.push_back('k');
		params += " " + _key;
	}
	if (_userLimit > 0) {
		modeStr.push_back('l');
		std::ostringstream oss;
        oss << _userLimit;
		params += " " + oss.str();
	}
	if (modeStr.empty())
		return "";
	return "+" + modeStr + params;
}

bool		Channel::isInviteOnly() const {return _inviteOnly;}
bool		Channel::isTopicRestricted() const {return _topicRestricted;}
bool		Channel::isOperator(Client* client) {return std::find(_operators.begin(), _operators.end(), client) != _operators.end();}
bool		Channel::isInInviteList(Client *client) {
	for (size_t i = 0; i < _invite.size(); i++){
		std::cout << _invite[i]->getNickname() << std::endl;
		if (_invite[i]->getFds() == client->getFds())
			return true;
	}
	return false;
}

void	Channel::setTopic(std::string topic) { _topic = topic; }
void	Channel::setTopicWriter(std::string topicW) { _topicWriter = topicW; }
void	Channel::setTopicTime(time_t newTime) { _topicTime = newTime; }
void	Channel::setInviteOnly(bool flag) { _inviteOnly = flag; }
void	Channel::setTopicRestricted(bool flag) { _topicRestricted = flag; }
void	Channel::setKey(const std::string & key) { _key = key; }
void	Channel::setUserLimit(int limit) { _userLimit = limit; }
void	Channel::setOperator(Client* client, bool flag) {
    if (flag) {
        if (std::find(_operators.begin(), _operators.end(), client) == _operators.end())
            _operators.push_back(client);
    } else {
        std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), client);
        if (it != _operators.end())
            _operators.erase(it);
    }
}

void	Channel::addInviteList(Client *client, bool flag) {
	if (flag == true) {
		if (std::find(_invite.begin(), _invite.end(), client) == _invite.end())
			_invite.push_back(client);
	}
	else
	{
		std::vector<Client *>::iterator it = std::find(_invite.begin(), _invite.end(), client);
		if (it != _invite.end())
			_invite.erase(it);
	}
}

void Channel::addClient(Client *client) {
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] == client)
            return;
    }
    _clients.push_back(client);
}

void Channel::removeClient(Client *client) {
	std::vector<Client *>::iterator it = std::find(_clients.begin(), _clients.end(), client);
	if (it != _clients.end())
	{
		_clients.erase(it);
		removeOperator(client);
		addInviteList(client, false);
	}
}

void	Channel::removeOperator(Client *client) {
	std::vector<Client *>::iterator it = std::find(_operators.begin(), _operators.end(), client);
	if (it != _operators.end())
		_operators.erase(it);
}

bool	Channel::hasClient(Client *client) {
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i] == client)
			return true;
	}
	return false;
}

bool	Channel::channelName(std::string channelName) {
	if (!channelName.empty() && channelName.size() > 1 && channelName[0] == '#')
		return true;
	return false;
}

void	Channel::sendChannelMessage(Client *client, std::string message) {
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i] != client) {
			send(_clients[i]->getFds(), message.c_str(), strlen(message.c_str()), 0);
			std::cout << BLU << "📤 [SEND] [->" << this->getName() << "] " << message << RESET;
		}
	}
}
