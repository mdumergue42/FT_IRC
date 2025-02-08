/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 11:31:12 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/08 14:01:09 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVER_HPP
#define	SERVER_HPP

#include <string>

class	Server {
	public :
		Server(void);
		Server(Server const &);
		~Server(void);
		Server & operator=(Server const &);

	private :
		std::string	_password;
		std::string	_port;
};

#endif
