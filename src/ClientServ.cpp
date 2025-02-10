#include <iostream>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int ac, char **av) {
	int port = 8080;
	if (ac == 2)
		port = std::atoi(av[1]);

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Erreur lors de la création du socket" << std::endl;
        return 1;
    }

    sockaddr_in adServ;
    adServ.sin_family = AF_INET;
    adServ.sin_port = htons(port);
	adServ.sin_addr.s_addr = INADDR_ANY;

    if (connect(clientSocket, (struct sockaddr *)&adServ, sizeof(adServ)) < 0) {
        std::cerr << "Connexion échouée" << std::endl;
        return 1;
    }

    std::cout << "Connecté au serveur sur le port " << port << std::endl;
    std::string message;
	std::cout << "Client:: ";
    while (std::getline(std::cin, message)) {
		if (message == "EXIT") {
            std::cout << "\033[33mDéconnexion...\033[0m" << std::endl;
            break;
        }
        message += "\r\n";
        ssize_t sentBytes = send(clientSocket, message.c_str(), message.size(), 0);
        if (sentBytes < 0)
            std::cerr << "\033[31mErreur lors de l'envoi du message.\033[0m" << std::endl;
        else
            std::cout << "\033[32mMessage envoyé\033[0m" << std::endl;
		std::cout << "Client:: ";
    }

    close(clientSocket);
    return 0;
}

