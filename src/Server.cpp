/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 11:31:12 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/08 13:04:09 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVER_CPP
#define	SERVER_CPP

#include <string>

class	Server {
	public :
		Server(void);
		Server(Server const &);
		~Server(void);
		Server & operator=(Server const &);

	private :
		std::string	_password;
		std::string	_nickname;
};

#endif
