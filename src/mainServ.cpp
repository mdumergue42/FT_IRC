/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mainServ.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madumerg <madumerg@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 10:27:38 by madumerg          #+#    #+#             */
/*   Updated: 2025/02/08 09:19:48 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int	main(void)
{
	std::cout << "Hello World" << std::endl;

	int	serverSocket = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM -> type socket TCP

	sockaddr_in adServ; // sockaddr_in -> stock add du socket

	adServ.sin_family = AF_INET; // AF_INET -> protocole IPv4
	adServ.sin_port = htons(8080);
	adServ.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY -> ecouter tt les IP dispo

	bind(serverSocket, (struct sockaddr *)&adServ, sizeof(adServ));
	listen(serverSocket, 42);

	int	clientSocket = accept(serverSocket, nullptr, nullptr);

	char buffer[1024] = {0};
	recv(clientSocket, buffer, sizeof(buffer), 0);
	std::cout << "Message du client : " << buffer << std::endl;

	close(serverSocket);

	return 0;
}
