/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: basverdi <basverdi@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 16:00:45 by basverdi          #+#    #+#             */
/*   Updated: 2025/02/24 15:00:38 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"

void	signalHandler(int signum) {
	(void)signum;
	//	exit(signum);
	}

const std::string	codeErr(std::string code) {
	std::string final = RED + ":localhost " + code + " " + COLOR_RESET;
	return final;
}

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
