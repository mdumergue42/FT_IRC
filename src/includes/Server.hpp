/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 11:31:12 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/08 18:18:49 by basverdi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVER_HPP
#define	SERVER_HPP

#include "Libs.hpp"

class	Server {
	public :
		Server(void);
		Server(std::string port, std::string password);
		Server(Server const &);
		~Server(void);
		Server & operator=(Server const &);

		void	initserv();
		void	run();
		void	NewClient();
	private :
		int							_port;
		std::string					_password;
		int							_server_fd;
		std::vector<struct pollfd>	_pollfds;
};

#endif
