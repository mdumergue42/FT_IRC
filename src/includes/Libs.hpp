/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: basverdi <basverdi@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 15:48:28 by basverdi          #+#    #+#             */
/*   Updated: 2025/02/21 07:05:33 by madumerg         ###   ########.fr       */
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

void	signalHandler(int);
const std::string	codeErr(std::string);

//CAP PASS 461 462 464
const std::string ERR_NEEDMOREPARAMS = " :Not enough parameters\r\n";
const std::string ERR_ALREADYREGISTERED = " :You may not reregister\r\n";
const std::string ERR_PASSWDMISMATCH = " :Password incorrect\r\n";

//NICK 431 432 433
const std::string ERR_NONICKNAMEGIVEN = " :No nickname given\r\n";
const std::string ERR_ERRONEUSNICKNAME = " :Erroneus nickname\r\n";
const std::string ERR_NICKNAMEINUSE = " :Nickname is already in use\r\n";

//USER *461 *462

//JOIN 403 475 471 473 _ 332 333 353 366 | *461
const std::string ERR_NOSUCHCHANNEL = " :No such channel\r\n";
const std::string ERR_BADCHANNELKEY = " :Cannot join channel (+k)\r\n";
const std::string ERR_CHANNELISFULL = " :Cannot join channel (+l)\r\n";
const std::string ERR_INVITEONLYCHAN = " :Cannot join channel (+i)\r\n";

const std::string RPL_ENDOFNAMES = " :End of /NAMES list\r\n";
//#define	RPL_TOPIC " :<topic>";
//#define	RPL_TOPICWHOTIME "<client> <channel> <nick> <setat>";
//#define	RPL_NAMREPLY  "<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"

//TOPIC 442 482 | *461 *403 *331 *332 *333
const std::string ERR_NOTONCHANNEL = " :You're not on that channel\r\n";
const std::string ERR_CHANOPRIVSNEEDED = " :You're not channel operator\r\n";
const std::string RPL_NOTOPIC = " :No topic is set\r\n";

//INVITE 341 443 | *461 *403 *442 *482
//#define RPL_INVITING "<client> <nick> <channel>"
const std::string ERR_USERONCHANNEL = " :is already on channel\r\n";
//reste avec invite list rien compris

//KICK 441 | *461 *403 *482 *442
const std::string ERR_USERNOTINCHANNEL = " :They aren't on that channel\r\n";

//MODE 401 404 411 412
const std::string ERR_NOSUCHNICK = " :No such nick/channel\r\n";
const std::string ERR_CANNOTSENDTOCHAN = " :Cannot send to channel\r\n";
const std::string ERR_NORECIPIENT = " :No recipient given\r\n";
const std::string ERR_NOTEXTTOSEND = " :No text to send\r\n";
const std::string ERR_UNKNOWNMODE = " : is unknown mode char to me\r\n";

//PRIVMSG *401 *404 *411 *412


#ifdef __linux
	#define nullptr 0
#endif

#endif
