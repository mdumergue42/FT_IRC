/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 17:25:44 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/21 07:26:10 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CLIENT_HPP
#define	CLIENT_HPP

#include "Libs.hpp"

class	Client {
	public :
		Client(void);
		Client(int);
		Client(Client const &);
		~Client(void);
		Client &	operator=(Client const &);

		///// Getters /////
		int				getFds(void) const;
		std::string		getUsername(void) const;
		std::string		getNickname(void) const;
		std::string&	getBuffer(void);
		bool			isOp(void) const;
		bool			isAuth(void) const;
		bool			isAlreadyNick(void) const;

		///// Setters /////

		void	setUsername(std::string);
		void	setNickname(std::string);
		void	setOp(bool);
		void	setAuth(bool);
		void	setAlreadyNick(bool);


	private :
		int		_fds;
		bool	_isOperator;
		bool	_isAuth;
		bool	_isAlreadyNick;

		std::string	_username;
		std::string	_nickname;
		std::string	_buf;
		
};

#endif
