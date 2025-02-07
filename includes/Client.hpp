/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 10:14:02 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/07 10:17:42 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CLIENT_HPP
#define	CLIENT_HPP

#include <string>

class	Client {

	public :
		Client(void);
		Client(std::string, std::string);
		Client(Client const &);
		~Client(void);
		Client &	operator=(Client const &);

	private :
		std::string	_nickname;
		std::string	_username;
};

#endif
