/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 09:20:49 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/08 09:25:50 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>


int	main(void)
{
	int	clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	
	sockaddr_in adServ;

	adServ.sin_family = AF_INET;
	adServ.sin_port = htons(8080);
	adServ.sin_addr.s_addr = INADDR_ANY;

	connect(clientSocket, (struct sockaddr *)&adServ, sizeof(adServ));

	const char *mess = "Hello Server !\n";
	send(clientSocket, mess, strlen(mess), 0);

	close(clientSocket);
	return 0;
}
