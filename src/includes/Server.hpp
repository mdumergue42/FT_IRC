/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 11:31:12 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/12 13:26:49 by madumerg         ###   ########.fr       */
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

		Client*						getFdsClient(int);
		Client*						getClientByNickname(int, std::string);
		Channel*					getChannel(const std::string &name);
		bool						isTaken(int, std::string);
		void						sendErrMess(int &, std::string);
		std::vector<std::string>	splitCom(const std::string &input);
		void						processCommand(Client* client, int fd, const std::string &command);
		void						removeClient(int fd);

		void						handlePass(Client* client, int fd, const std::vector<std::string>& tokens);
		void						handleNick(Client* client, int fd, const std::vector<std::string>& tokens);
		void						handleUser(Client* client, int fd, const std::vector<std::string>& tokens);
		void						handleJoin(Client* client, int fd, const std::vector<std::string>& tokens);
		void						handleKick(Client*, int, const std::vector<std::string>&);
		void						handleInvite(Client*, int, const std::vector<std::string>&);

	private :
		int							_port;
		std::string					_password;
		int							_server_fd;
		std::vector<struct pollfd>	_pollfds;
		std::vector<Client *>		_clientfds;
		std::map<Client *, std::string>	_clientByN;
		std::vector<Channel*>		_channels;
		
		typedef void (Server::*CommandFunc)(Client* client, int fd, const std::vector<std::string>& tokens);
		std::map<std::string, CommandFunc> _commandMap;
};

#endif
