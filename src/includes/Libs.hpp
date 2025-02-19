/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: basverdi <basverdi@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 15:48:28 by basverdi          #+#    #+#             */
/*   Updated: 2025/02/19 18:56:20 by madumerg         ###   ########.fr       */
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
//CAP PASS 461 462 464
#define	ERR_NEEDMOREPARAMS " :Not enough parameters"
#define	ERR_ALREADYREGISTERED " :You may not reregister"
#define	ERR_PASSWDMISMATCH " :Password incorrect"

//NICK 431 432 433
#define ERR_NONICKNAMEGIVEN " :No nickname given";
#define	ERR_ERRONEUSNICKNAME " :Erroneus nickname";
#define	ERR_NICKNAMEINUSE " :Nickname is already in use";

//USER 461 462

//JOIN *461 403 475 471 473 476 332 333 353 366
#define	ERR_NOSUCHCHANNEL " :No such channel";
#define	ERR_BADCHANNELKEY " :Cannot join channel (+k)";
#define	ERR_CHANNELISFULL " ";


#ifdef __linux
	#define nullptr 0
#endif

#endif
