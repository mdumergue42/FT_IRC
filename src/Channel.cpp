/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 19:43:52 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/11 00:54:45 by bastienverdie    ###   ########.fr       */
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

std::string Channel::getName() const {
	return _name;
}

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
