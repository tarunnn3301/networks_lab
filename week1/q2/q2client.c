
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Function to display error messages and exit
void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int clientSocket, serverPort, bytesSent;
    struct sockaddr_in serverAddress;
    struct hostent *serverHost;
    char buffer[256];

    // Check for correct command line arguments
    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1);
    }

    // Convert provided port string to an integer
    serverPort = atoi(argv[2]);

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        error("ERROR opening socket");
    }

    // Get server information by hostname
    serverHost = gethostbyname(argv[1]);
    if (serverHost == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    // Clear serverAddress struct and set necessary fields
    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *) serverHost->h_addr, (char *) &serverAddress.sin_addr.s_addr, serverHost->h_length);
    serverAddress.sin_port = htons(serverPort);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        error("ERROR connecting");
    }
    printf("Connected to the server\n");

    while (1) {
        // Clear buffer and get input from user
        bzero(buffer, sizeof(buffer));
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character

        // Check if user wants to exit
        if (strcmp(buffer, "/exit") == 0) {
            break;
        }

        // Send user input to the server
        bytesSent = write(clientSocket, buffer, strlen(buffer));
        if (bytesSent < 0) {
            error("Error on writing");
        }

        // Clear buffer and read response from server
        bzero(buffer, sizeof(buffer));
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            error("Error on reading.");
        }

        // Display the server's response
        printf("Server: %s", buffer);
    }

    // Close the socket and exit
    close(clientSocket);
    return 0;
}
