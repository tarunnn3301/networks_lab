
#include "base_64_decoder.c"
#include "base_64_encoder.c"
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 

void handleClientConnection(int clientSocket, struct sockaddr_in *clientAddress)
{
	char *clientIP = inet_ntoa(clientAddress->sin_addr); // IP address of the client
	int clientPort = clientAddress->sin_port; // Port of the client
	printf("\nNEW CLIENT CONNECTION (%s : %d) ESTABLISHED\n", clientIP, clientPort);

	char buffer[1500]; // Buffer to store data

	// Wait for messages from the client until a close connection request is received
	while (1)
	{
		bzero(buffer, 1500); 
		int bytesRead = read(clientSocket, buffer, 1500); // Read message from the socket
		
		// The first character of the message indicates its type. '3' refers to a close connection request
		if (buffer[0] == '3')
			break;

		if (buffer[0] == '1')
		{
			printf("\nMessage received from client %s : %d\n\tEncoded Message: %s\n", clientIP, clientPort, buffer + 1); // Print encoded message
			printf("\tDecoded Message: %s\n", decode(buffer + 1)); // Print decoded message

			bzero(buffer, 1500);
			buffer[0] = '2'; // Set message type to acknowledge (2)
			strcpy(buffer + 1, encode("Acknowledgement from server")); // Append encoded acknowledgement message
			write(clientSocket, buffer, strlen(buffer)); // Send acknowledgement to the client
		}
		else
			break;
	}

	close(clientSocket); // Close the client connection
	printf("\nCLIENT CONNECTION (%s : %d) CLOSED\n", clientIP, clientPort);
	exit(0);
}

int main(int argc, char *argv[])
{
	struct sockaddr_in serverAddress, clientAddress;
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET is for IPv4, SOCK_STREAM for TCP socket
	
	// Check if socket creation failed
	if (serverSocket == -1)
	{ 
		printf("\nSOCKET CREATION FAILURE\n");
		exit(0);
	}
	
	serverAddress.sin_family = AF_INET; // Address family: IPv4
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Bind to all available local interfaces
	serverAddress.sin_port = htons(atoi(argv[1])); // Port number provided by user
	memset(&serverAddress.sin_zero, 8, 0); 
	socklen_t addressLength = sizeof(struct sockaddr_in);
	
	// Check if binding to port failed
	if (bind(serverSocket, (struct sockaddr*) &serverAddress, addressLength) < 0)
	{ 
		printf("\nBINDING FAILED\n");
		exit(0);
	}

	// Listen to the current socket
	if (listen(serverSocket, 20) == -1)
	{
		printf("\nLISTEN FAILED\n");
		exit(0);
	}
	printf("SERVER WORKING\n");
	fflush(stdout);
	
	while (1)
	{
		int clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &addressLength); // Client tries to connect
		
		// Check if client connection was established
		if (clientSocket < 0)
		{ 
			printf("\nSERVER-CLIENT CONNECTION COULD NOT BE ESTABLISHED\n");
			exit(0);
		}
		
		int forkStatus = fork(); // Fork to create a child process to handle this client, enabling concurrent handling of multiple clients
		switch (forkStatus)
		{
			// Error while creating child process
	 		case -1:
				printf("\nCOULDN'T ESTABLISH CONNECTION\n");
				break;
			
			// Child process
			case 0:
				close(serverSocket); // The server socket is managed by the parent process
				handleClientConnection(clientSocket, &clientAddress); // This function handles the client's communication
				break;
			
			// Parent process
			default:
				close(clientSocket); // The client socket is managed by the child process	
		}
	}
	return 0;
}
