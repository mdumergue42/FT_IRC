/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 09:20:49 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/08 12:21:57 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

//ce fichier ne sert a rien c'est juste pour test

int	main(void)
{
	int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	
	sockaddr_in adServ;

	adServ.sin_family = AF_INET;
	adServ.sin_port = htons(6667);
	adServ.sin_addr.s_addr = INADDR_ANY;

	connect(clientSocket, (struct sockaddr *)&adServ, sizeof(adServ));

	std::string messs;
	std::getline(std::cin, messs);
	const char * mess = messs.c_str();
	send(clientSocket, mess, strlen(mess), 0);

	close(clientSocket);
	return 0;
}
