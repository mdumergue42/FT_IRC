/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 19:43:52 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/14 14:51:16 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Channel.hpp"

Channel::Channel( void ) {}

Channel::Channel(std::string name) :
	_name(name) {}

Channel::Channel( Channel const & copy ) {*this = copy;}

Channel::~Channel( void ) {}

Channel & Channel::operator=( Channel const & op ) {
	if (this != &op)
		return *this;
	return *this;
}

std::string Channel::getName() const {return _name;}
std::string	Channel::getTopic() const {return _topic;}

void	Channel::setTopic(std::string topic) {_topic = topic;}

void Channel::addClient(Client *client) {
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] == client)
            return;
    }
    _clients.push_back(client);
}

void Channel::removeClient(Client *client) {
    for (size_t i = 0; i < _clients.size(); i++) {
        if (_clients[i] == client) {
            _clients.erase(_clients.begin() + i);
            return;
        }
    }
}

bool	Channel::hasClient(Client *client) {
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i] == client)
			return true;
	}
	return false;
}
