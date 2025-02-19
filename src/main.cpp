/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: basverdi <basverdi@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 16:00:45 by basverdi          #+#    #+#             */
/*   Updated: 2025/02/19 18:55:41 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"

void	signalHandler(int signum) {
	//clear fd client
	exit(signum);}

int main(int ac, char **av) {
	if (ac != 3)
		std::cout << "Erreur fdp !" << std::endl;
//	try
//	{
	Server irc_server(av[1], av[2]);

	signal(SIGINT, signalHandler);
	irc_server.initserv();
	irc_server.run();
//	}
//	catch (std::exception & e)
//	{
//		std::cout << e.what() << std::endl; 
//	}
}
