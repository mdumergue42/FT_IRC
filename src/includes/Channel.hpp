/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 19:42:09 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/17 20:36:35 by madumerg         ###   ########.fr       */
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
		std::string	getTopic() const;
		bool		isInviteOnly() const { return _inviteOnly; }
		bool		isTopicRestricted() const { return _topicRestricted; }
		std::string getKey() const { return _key; }
		int			getUserLimit() const { return _userLimit; }
		
		void	setTopic(std::string);
		void	setInviteOnly(bool flag);
		void	setTopicRestricted(bool flag);
		void	setKey(const std::string & key);
		void	setUserLimit(int limit);
		void	setOperator(Client* client, bool flag);
		
		void		addClient(Client *client);
		void		removeClient(Client *client);
		bool		hasClient(Client *);

		bool		channelName(std::string);
		void		sendChannelMessage(Client*, std::string);

	private:
		std::string _name;
		std::string _topic;
		std::vector<Client*> _clients;

		bool _inviteOnly;
		bool _topicRestricted;
		std::string _key;
	    int _userLimit;
	    std::vector<Client*> _operators;
};

#endif
