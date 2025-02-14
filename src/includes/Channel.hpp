/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 19:42:09 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/14 12:49:37 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CHANNEL_HPP
#define	CHANNEL_HPP

#include "Client.hpp"

class	Channel {
	public:
		Channel(void);
		Channel(std::string name);
		Channel(Channel const &);
		~Channel(void);
		Channel & operator=(Channel const &);

		std::string getName() const;
		void		addClient(Client *client);
		void		removeClient(Client *client);
		bool		hasClient(Client *);


	private:
		std::string _name;
		std::string _topic;
		std::vector<Client*> _clients;
};

#endif
