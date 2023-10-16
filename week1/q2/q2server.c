
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 10 // Maximum number of simultaneous clients

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Port not provided, program terminated.\n");
        exit(1);
    }

    int sockfd, newsockfd, portno, n;
    char buffer[255];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    int client_sockets[MAX_CLIENTS];
    fd_set readfds;
    int max_clients = 0;

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket.");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket to the specified port
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Binding failed");

    // Listen for incoming connections
    listen(sockfd, 5);

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
    int max_sd = sockfd;

    // Initialize client sockets array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    while (1) {
        fd_set tempfds = readfds;

        // Use select to monitor socket activity
        if (select(max_sd + 1, &tempfds, NULL, NULL, NULL) < 0) {
            error("Select error.");
        }

        // Check if there's a new incoming connection
        if (FD_ISSET(sockfd, &tempfds)) {
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
            if (newsockfd < 0) {
                error("Error on Accept");
            }

            // Add the new client socket to the array
            if (max_clients < MAX_CLIENTS) {
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_sockets[i] == 0) {
                        client_sockets[i] = newsockfd;
                        break;
                    }
                }
                if (newsockfd > max_sd) {
                    max_sd = newsockfd;
                }
                FD_SET(newsockfd, &readfds);
                max_clients++;
                printf("New client connected, socket fd is %d\n", newsockfd);
            } else {
                printf("Maximum number of clients reached. Rejected.\n");
                close(newsockfd);
            }
        }

        // Check each client socket for incoming data
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &tempfds)) {
                bzero(buffer, sizeof(buffer));
                n = read(sd, buffer, sizeof(buffer));
                if (n <= 0) {
                    printf("Client %d disconnected.\n", sd);
                    close(sd);
                    FD_CLR(sd, &readfds);
                    client_sockets[i] = 0;
                    max_clients--;
                } else {
                    printf("Received from client %d: %s\n", sd, buffer);
                    printf("Enter response to send back to client %d: ", sd);
                    bzero(buffer, sizeof(buffer));
                    fgets(buffer, sizeof(buffer), stdin);
                    n = write(sd, buffer, strlen(buffer));
                    if (n < 0) {
                        error("Error on writing.");
                    }
                }
            }
        }
    }

    close(sockfd);
    return 0;
}
