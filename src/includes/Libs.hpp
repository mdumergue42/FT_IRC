/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: basverdi <basverdi@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 15:48:28 by basverdi          #+#    #+#             */
/*   Updated: 2025/03/07 15:08:09 by baverdi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBS_H
# define LIBS_H

#include "Colors.h"
#include <string>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/poll.h>
#include <poll.h>
#include <vector>
#include <exception>
#include <stdexcept>
#include <fcntl.h>
#include <map>
#include <errno.h>
#include <algorithm>
#include <csignal>
#include <sstream>
#include <ctime>

void	signalHandler(int);
const std::string	codeErr(std::string);

//001 002 003 004 005
const std::string RPL_WELCOME = " :Welcome to the ircserv, ";
//002
const std::string RPL_YOURHOST = " :Your host is ircserv, running version 0.1\r\n";
//003
const std::string RPL_CREATED = " :This server was created ";
//004
const std::string RPL_MYINFO = " ircserv 0.1 o itkol\r\n";
//005
const std::string RPL_ISUPPORT = " NICKLEN=9 CHANTYPES=# :are supported by this server\r\n";

//331
const std::string RPL_NOTOPIC = " :No topic is set\r\n";

//366
const std::string RPL_ENDOFNAMES = " :End of /NAMES list\r\n";

//401
const std::string ERR_NOSUCHNICK = " :No such nick/channel\r\n";
//404
const std::string ERR_CANNOTSENDTOCHAN = " :Cannot send to channel\r\n";
//403 
const std::string ERR_NOSUCHCHANNEL = " :No such channel\r\n";

//411
const std::string ERR_NORECIPIENT = " :No recipient given\r\n";
//412
const std::string ERR_NOTEXTTOSEND = " :No text to send\r\n";

//431
const std::string ERR_NONICKNAMEGIVEN = " :No nickname given\r\n";
//432
const std::string ERR_ERRONEUSNICKNAME = " :Erroneus nickname\r\n";
//433
const std::string ERR_NICKNAMEINUSE = " :Nickname is already in use\r\n";

//441
const std::string ERR_USERNOTINCHANNEL = " :They aren't on that channel\r\n";
//442
const std::string ERR_NOTONCHANNEL = " :You're not on that channel\r\n";
//443
const std::string ERR_USERONCHANNEL = " :is already on channel\r\n";

//461
const std::string ERR_NEEDMOREPARAMS = " :Not enough parameters\r\n";
//462
const std::string ERR_ALREADYREGISTERED = " :You may not reregister\r\n";
//464
const std::string ERR_PASSWDMISMATCH = " :Password incorrect\r\n";

//471
const std::string ERR_CHANNELISFULL = " :Cannot join channel (+l)\r\n";
//472
const std::string ERR_UNKNOWNMODE = " : is unknown mode char to me\r\n";
//473
const std::string ERR_INVITEONLYCHAN = " :Cannot join channel (+i)\r\n";
//475
const std::string ERR_BADCHANNELKEY = " :Cannot join channel (+k)\r\n";

//482
const std::string ERR_CHANOPRIVSNEEDED = " :You're not channel operator\r\n";

#define USERLEN 9

#ifdef __linux
	#define nullptr 0
#endif

#endif
