#include "base_64_decoder.c"
#include "base_64_encoder.c"
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 

// Function to get user input
void getInput(char inputBuffer[])
{
	char inputChar;
	bzero(inputBuffer, 1500); 
	int i = 0;
	
	// Maximum input size is 1000 characters
	while (1)
	{
		scanf("%c", &inputChar); // Read message character by character    
		if (inputChar == '\n') 
			break;
		if (i == 1000)
		{
			printf("Only the first 1000 characters of the message are being sent.\n");
			break;
		}
		inputBuffer[i++] = inputChar; 
	}
}

// Function to close the connection
void closeConnection(char buffer[], int clientSocket)
{
	buffer[0] =  '3'; 	 // Set message type
	strcpy(buffer+1, encode("connection_close")); // Append encoded value of "connection_close" after message type
	printf("CONNECTION CLOSED\n"); 
	write(clientSocket, buffer, strlen(buffer)); // Write to socket
}

int main(int argc, char *argv[])
{
	struct sockaddr_in server; 
	char userInput; // To read user input character by character 
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET for IPv4, SOCK_STREAM for TCP socket
	char buffer[1500]; // Buffer to store receiving and modified sending values
	
	// Check if socket creation failed
	if (clientSocket == -1)
	{
		printf("SOCKET CREATION FAILURE\n");
		return 0;
	} 

	server.sin_family = AF_INET; // Address family: IPv4

	// Get server IP address from user input
	if (inet_aton(argv[1], &server.sin_addr) == 0)
	{ 
		printf("SERVER IP ADDRESS ERROR\n");
		return 0;
	} 

	int serverPort = atoi(argv[2]); // Get server port from user input
	server.sin_port = htons(serverPort); 
	socklen_t length = sizeof(struct sockaddr_in); 

	// Check if connection to server failed
	if (connect(clientSocket, (struct sockaddr *)&server, length) == -1)
	{ 
		printf("COULDN'T CONNECT TO THE SERVER\n");
		exit(0);
	} 

	while (1)
	{
		printf("Press 1 for TYPE:1 message. Press 3 to close the connection.\n");
		scanf(" %c", &userInput); // Adding a space before %c to consume newline character

		if (userInput == '3')
		{
			closeConnection(buffer, clientSocket);
			break; 
		}
		else if (userInput == '1')
		{
			printf("Enter the TYPE-1 message: ");
			getchar(); // Consume newline left by previous scanf
			getInput(buffer);

			strcpy(buffer + 1, encode(buffer)); // Append encoded value of message after message type
			buffer[0] = '1'; // Set message type to 1
			write(clientSocket, buffer, strlen(buffer)); // Write to socket
			bzero(buffer, 1500);
			int received = read(clientSocket, buffer, 50); // Receive acknowledgement from server
			strcpy(buffer + 1, decode(buffer + 1)); // Decode message

			// Message type 2 indicates acknowledgement
			if (buffer[0] != '2')
			{
				printf("Acknowledgement could not be received.\nResend the message.\n"); 
				continue; 
			}

			printf("TYPE-2: %s with IP %s and port %d\n",buffer+1, argv[1], serverPort); 
		}
	}

	close(clientSocket); // Close the socket

	return 0;
}
