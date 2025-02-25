/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: basverdi <basverdi@42angouleme.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 16:00:45 by basverdi          #+#    #+#             */
/*   Updated: 2025/02/25 16:43:51 by madumerg         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Server.hpp"

void	signalHandler(int signum) {
		exit(signum);
	}

const std::string	codeErr(std::string code) {
	std::string final = ":localhost " + code + " ";
	return final;
}

int main(int ac, char **av) {
	if (ac != 3)
		std::cout << "Erreur fdp !" << std::endl;
	Server irc_server(av[1], av[2]);

	signal(SIGINT, signalHandler);
	irc_server.initserv();
	irc_server.run();
}
