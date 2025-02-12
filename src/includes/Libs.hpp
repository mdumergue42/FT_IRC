/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libs.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: basverdi <basverdi@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 15:48:28 by basverdi          #+#    #+#             */
/*   Updated: 2025/02/12 03:25:48 by bastienverdie    ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBS_H
# define LIBS_H

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

#ifdef __linux
	#define nullptr 0
#endif

#endif
