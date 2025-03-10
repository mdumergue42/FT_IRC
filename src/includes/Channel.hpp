/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/10 19:42:09 by madumerg          #+#    #+#             */
/*   Updated: 2025/03/05 04:20:04 by baverdi          ###   ########.fr       */
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
		const std::vector<Client*>& getClients() const;
		std::string	getTopic() const;
		std::string	getTopicWriter() const;
		std::string	getTopicTime() const;
		std::string getKey() const;
		long unsigned int getUserLimit() const;
		std::string getModes() const;
		std::string getModesOps() const;

		bool		isOperator(Client*);
		bool		isInviteOnly() const;
		bool		isTopicRestricted() const;
		bool		isInInviteList(Client*);
		
		void		setTopic(std::string);
		void		setTopicWriter(std::string);
		void		setTopicTime(time_t);
		void		setInviteOnly(bool flag);
		void		setTopicRestricted(bool flag);
		void		setKey(const std::string & key);
		void		setUserLimit(int limit);
		void		setOperator(Client* client, bool flag);
		void		addInviteList(Client*, bool);
		
		void		addClient(Client *client);
		void		removeClient(Client *client);
		void		removeOperator(Client *);
		bool		hasClient(Client *);

		bool		channelName(std::string);
		void		sendChannelMessage(Client*, std::string);

	private:
		std::string _name;
		std::string _topic;
		std::string	_topicWriter;
		time_t		_topicTime;
		std::vector<Client*> _clients;

		bool _inviteOnly;
		bool _topicRestricted;
		std::string _key;
	    long unsigned int _userLimit;
	    std::vector<Client*> _operators;
		std::vector<Client*> _invite;
};

#endif
