/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 17:24:36 by madumerg          #+#    #+#             */
/*   Updated: 2025/03/02 05:04:28 by baverdi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Client.hpp"

Client::Client( void ) {}

Client::Client( int fds ) : 
	_fds(fds),
	_nickname("*") {
		_isAuth = false;
		_isRegistered = false;
}

Client::Client( Client const & copy ) {*this = copy;}

Client::~Client( void ) {}

Client &	Client::operator=( Client const & op ) {
	if (this != &op)
	{
		this->_fds = op.getFds();
		this->_username = op.getUsername();
		this->_nickname = op.getNickname();
		this->_isAuth = op.isAuth();
		this->_isOperator = op.isOp();
	}
	return *this;
}


/*Setters*/

void	Client::setNickname( std::string nickname ) {_nickname = nickname;}
void	Client::setUsername( std::string username ) {_username = username;}
void	Client::setOp( bool op ) {_isOperator = op;}
void	Client::setAuth( bool auth ) {_isAuth = auth;}
void	Client::setRegister(bool regi) {_isRegistered = regi;};

/*Getters*/

int				Client::getFds( void ) const {return _fds;}
std::string		Client::getUsername( void ) const {return _username;}
std::string		Client::getNickname( void ) const {return _nickname;}
std::string&	Client::getBuffer( void ) {return _buf;}
bool			Client::isAuth( void ) const {return _isAuth;}
bool			Client::isRegistered(void) const {return _isRegistered;}
bool			Client::isOp( void ) const {return _isOperator;};
