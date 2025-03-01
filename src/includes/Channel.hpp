/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 19:42:09 by madumerg          #+#    #+#             */
/*   Updated: 2025/03/01 14:11:24 by madumerg         ###   ########.fr       */
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
		std::string	getTopicWriter() const;
		std::string getKey() const;
		int			getUserLimit() const;
		bool		isInviteOnly() const;
		bool		isTopicRestricted() const;
		bool		isInInviteList(Client*);
		
		void		setTopic(std::string);
		void		setTopicWriter(std::string);
		void		setInviteOnly(bool flag);
		void		setTopicRestricted(bool flag);
		void		setKey(const std::string & key);
		void		setUserLimit(int limit);
		void		setOperator(Client* client, bool flag);
		void		addInviteList(Client*, bool);
		
		void		addClient(Client *client);
		void		removeClient(Client *client);
		bool		hasClient(Client *);

		bool		channelName(std::string);
		void		sendChannelMessage(Client*, std::string);

	private:
		std::string _name;
		std::string _topic;
		std::string	_topicWriter;
		std::vector<Client*> _clients;

		bool _inviteOnly;
		bool _topicRestricted;
		std::string _key;
	    int _userLimit;
	    std::vector<Client*> _operators;
		std::vector<Client*> _invite;
};

#endif
