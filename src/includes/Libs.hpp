/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: basverdi <basverdi@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 15:48:28 by basverdi          #+#    #+#             */
/*   Updated: 2025/02/20 11:03:31 by madumerg         ###   ########.fr       */
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

#define	SERVER ":irc_server "

//CAP PASS 461 462 464
const std::string ERR_NEEDMOREPARAMS = " :Not enough parameters";
const std::string ERR_ALREADYREGISTERED = " :You may not reregister";
const std::string ERR_PASSWDMISMATCH = " :Password incorrect";

//NICK 431 432 433
const std::string ERR_NONICKNAMEGIVEN = " :No nickname given";
const std::string ERR_ERRONEUSNICKNAME = " :Erroneus nickname";
const std::string ERR_NICKNAMEINUSE = " :Nickname is already in use";

//USER *461 *462

//JOIN 403 475 471 473 _ 332 333 353 366 | *461
const std::string ERR_NOSUCHCHANNEL = " :No such channel";
const std::string ERR_BADCHANNELKEY = " :Cannot join channel (+k)";
const std::string ERR_CHANNELISFULL = " :Cannot join channel (+l)";
const std::string ERR_INVITEONLYCHAN = " :Cannot join channel (+i)";

//#define	RPL_TOPIC " :<topic>";
//#define	RPL_TOPICWHOTIME "<client> <channel> <nick> <setat>";
//#define	RPL_NAMREPLY  "<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"

//TOPIC 442 482 | *461 *403 *331 *332 *333
const std::string ERR_NOTONCHANNEL = " :You're not on that channel";
const std::string ERR_CHANOPRIVSNEEDED = " :You're not channel operator";

//INVITE 341 443 | *461 *403 *442 *482
//#define RPL_INVITING "<client> <nick> <channel>"
const std::string ERR_USERONCHANNEL = " :is already on channel";
//reste avec invite list rien compris

//KICK 441 | *461 *403 *482 *442
const std::string ERR_USERNOTINCHANNEL = " :They aren't on that channel";

//MODE 401 404 411 412
const std::string ERR_NOSUCHNICK = " :No such nick/channel";
const std::string ERR_CANNOTSENDTOCHAN = " :Cannot send to channel";
const std::string ERR_NORECIPIENT = " :No recipient given (<command>)";
const std::string ERR_NOTEXTTOSEND = " :No text to send";
const std::string ERR_UNKNOWNMODE = " : is unknown mode char to me";

//PRIVMSG *401 *404 *411 *412


#ifdef __linux
	#define nullptr 0
#endif

#endif
