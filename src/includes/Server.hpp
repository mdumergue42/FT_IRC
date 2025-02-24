/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 11:31:12 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/24 11:00:11 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SERVER_HPP
#define	SERVER_HPP

#include "Libs.hpp"
#include "Client.hpp"
#include "Channel.hpp"

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

		/// Get ///

		Client*						getFdsClient(int);
		Client*						getClientByNickname(int, std::string);
		Channel*					getChannel(const std::string &);

		//// Utils ////

		bool						isTaken(int, std::string);
		std::vector<std::string>	splitCom(const std::string &);
		void						processCommand(Client*, int, const std::string &);
		std::vector<std::string>	targetSplit(std::string);

		void						removeClient(int);
		void						removeChannel();

		std::string					sendMess(int, std::string);
		void						sendServerMessage(Client*, std::string);

		///// Commands /////

		void		handlePass(Client*, int, const std::vector<std::string>&);
		void		handleNick(Client*, int, const std::vector<std::string>&);
		void		handleUser(Client*, int, const std::vector<std::string>&);
		void		handleJoin(Client*, int, const std::vector<std::string>&);
		void		handleKick(Client*, int, const std::vector<std::string>&);
		void		handleInvite(Client*, int, const std::vector<std::string>&);
		void		handleMode(Client*, int, const std::vector<std::string>&);
		void		handleTopic(Client*, int, const std::vector<std::string>&);
		void		handlePrivMsg(Client*, int, const std::vector<std::string>&);
		void		handleDie(Client *, int, const std::vector<std::string>&);
		void		handleQuit(Client *, int, const std::vector<std::string>&);

	private :
		int							_port;
		std::string					_password;
		int							_server_fd;
		std::vector<struct pollfd>	_pollfds;
		std::vector<Client *>		_clientfds;
		std::map<Client *, std::string>	_clientByN;
		std::vector<Channel*>		_channels;
		bool	_run;
		
		typedef void (Server::*CommandFunc)(Client* client, int fd, const std::vector<std::string>& tokens);
		std::map<std::string, CommandFunc> _commandMap;
};

#endif
