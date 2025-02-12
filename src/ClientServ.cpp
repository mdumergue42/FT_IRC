#include <iostream>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <cstdlib>

#ifdef __linux
	#define nullptr 0
#endif

// Structure pour sauvegarder les réglages originaux du terminal
struct TerminalSettings {
    struct termios original;
};

// Active le mode raw (non-canonique, sans echo)
void enableRawMode(TerminalSettings &settings) {
    tcgetattr(STDIN_FILENO, &settings.original);
    struct termios raw = settings.original;
    raw.c_lflag &= ~(ICANON | ECHO);  // Désactive la lecture canonique et l'echo
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

// Restaure les réglages originaux du terminal
void disableRawMode(const TerminalSettings &settings) {
    tcsetattr(STDIN_FILENO, TCSANOW, &settings.original);
}

int main(int argc, char **argv) {
    int port = 8080;
    if (argc == 2)
        port = std::atoi(argv[1]);

    // Création du socket client
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "\033[31mErreur lors de la création du socket\033[0m" << std::endl;
        return 1;
    }

    // Configuration de l'adresse du serveur
    sockaddr_in adServ;
    adServ.sin_family = AF_INET;
    adServ.sin_port = htons(port);
    // Pour tester localement, on utilise 127.0.0.1
    adServ.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connexion au serveur
    if (connect(clientSocket, (struct sockaddr *)&adServ, sizeof(adServ)) < 0) {
        std::cerr << "\033[31mConnexion échouée\033[0m" << std::endl;
        return 1;
    }

    std::cout << "\033[90mConnecté au serveur sur le port " << port << "\033[0m" << std::endl;

    // On passe le terminal en mode raw
    TerminalSettings termSettings;
    enableRawMode(termSettings);

    std::string inputBuffer = "";

    // Affiche le prompt initial
    std::cout << "\033[36mClient:: \033[0m" << std::flush;

    fd_set readfds;
    int maxfd = (clientSocket > STDIN_FILENO) ? clientSocket : STDIN_FILENO;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(clientSocket, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int activity = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            std::cerr << "\033[31m✗\033[0m\n\033[31mselect() a échoué\033[0m" << std::endl;
            break;
        }

        // Si des données sont disponibles sur STDIN (entrée utilisateur)
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            char c;
            ssize_t r = read(STDIN_FILENO, &c, 1);
            if (r < 0) {
                std::cerr << "\033[31m✗\033[0m\n\033[31mErreur de lecture sur STDIN\033[0m" << std::endl;
                break;
            } else if (r == 0) {
                break; // EOF
            } else {
                if (c == '\r' || c == '\n') {
                    // L'utilisateur a appuyé sur Enter
                    if (inputBuffer == "EXIT") {
                        std::cout << "\n\033[33mDéconnexion...\033[0m" << std::endl;
                        break;
                    }
                    std::string fullMessage = inputBuffer + "\r\n";
                    ssize_t sentBytes = send(clientSocket, fullMessage.c_str(), fullMessage.size(), 0);
                    // Efface la ligne actuelle et réécrit-la avec le symbole
                    if (sentBytes < 0) {
                        std::cout << "\033[2K\r" << "\033[36mClient:: \033[0m" << inputBuffer 
                                  << " \033[31m✗\033[0m" << std::flush;
                        std::cerr << "\n\033[31mErreur lors de l'envoi du message.\033[0m\n\033[36mClient:: \033[0m";
                    } else {
                        std::cout << "\033[2K\r" << "\033[36mClient:: \033[0m" << inputBuffer 
                                  << " \033[32m✓\033[0m" << std::flush;
                    }
                    inputBuffer.clear();
                    // On ne rajoute pas de nouvelle ligne prompt ici ; on attend qu'une nouvelle entrée ou une réponse du serveur l'affiche.
                } else if (c == 127 || c == '\b') { // Gestion du backspace
                    if (!inputBuffer.empty()) {
						inputBuffer.erase(inputBuffer.end() - 1);
                        std::cout << "\033[2K\r" << "\033[36mClient:: \033[0m" << inputBuffer << std::flush;
                    }
                } else {
                    inputBuffer.push_back(c);
                    std::cout << c << std::flush; // Affiche le caractère tapé
                }
            }
        }

        // Si des données arrivent depuis le serveur
        if (FD_ISSET(clientSocket, &readfds)) {
            char buffer[1024];
            ssize_t recvBytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (recvBytes < 0) {
                std::cerr << "\n\033[31mErreur lors de la réception depuis le serveur\033[0m" << std::endl;
            } else if (recvBytes == 0) {
                std::cout << "\n\033[33mLe serveur a fermé la connexion\033[0m" << std::endl;
                break;
            } else {
                buffer[recvBytes] = '\0';
                std::cout << "\n\033[35mServeur:: \033[0m" << buffer;
            }
            // Réaffiche le prompt et la saisie en cours (qui devrait être vide ici)
            std::cout << "\033[36mClient:: \033[0m" << inputBuffer << std::flush;
        }
    }

    disableRawMode(termSettings);
    close(clientSocket);
    return 0;
}
