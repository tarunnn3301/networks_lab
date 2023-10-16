#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

const int MAX_MESSAGE_LENGTH = 1024;

// Function to send a formatted message to the server
void sendMessageToServer(int clientSocket, int messageType, const char* message) {
    char fullMessage[MAX_MESSAGE_LENGTH];
    snprintf(fullMessage, sizeof(fullMessage), "%d:%s", messageType, message);

    send(clientSocket, fullMessage, strlen(fullMessage), 0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server_IP_Address> <Server_Port_Number>\n", argv[0]);
        return 1;
    }

    // Get server information from the provided hostname
    struct hostent* hostInfo = gethostbyname(argv[1]);
    if (hostInfo == NULL) {
        fprintf(stderr, "%s is unavailable\n", argv[1]);
        exit(1);
    }

    // Extract server IP address from host information
    struct in_addr* serverAddress = (struct in_addr*)hostInfo->h_addr;
    const char* serverIPAddress = inet_ntoa(*serverAddress);

    // Extract server port number from command line argument
    int serverPort = atoi(argv[2]);

    // Create a socket for the client
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        fprintf(stderr, "Error creating socket\n");
        return 1;
    }

    // Configure server socket address
    struct sockaddr_in serverSocketAddress;
    serverSocketAddress.sin_family = AF_INET;
    serverSocketAddress.sin_port = htons(serverPort);

    // Convert server IP address to binary form
    if (inet_pton(AF_INET, serverIPAddress, &serverSocketAddress.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address\n");
        close(clientSocket);
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverSocketAddress, sizeof(serverSocketAddress)) == -1) {
        fprintf(stderr, "Error connecting to server\n");
        close(clientSocket);
        return 1;
    }

    char inputType1[10];
    int inputType;
    char expression[MAX_MESSAGE_LENGTH];
    char ackData[MAX_MESSAGE_LENGTH];

    // Interaction loop with the server
    while (1) {
        printf("Enter Input Type (1: Expression, 3: Close): ");
        scanf("%s", inputType1);

        if (strcmp(inputType1, "1") != 0 && strcmp(inputType1, "3") != 0) {
            printf("Wrong Input type\n");
            continue;
        }

        inputType = atoi(inputType1);

        if (inputType == 3) {
            sendMessageToServer(clientSocket, inputType, "Closing communication");
            break;
        }

        printf("Enter Expression (expression format: operand1 operator operand2): ");
        getchar(); // Consume newline left in input buffer
        fgets(expression, sizeof(expression), stdin);
        expression[strcspn(expression, "\n")] = '\0';

        sendMessageToServer(clientSocket, inputType, expression);

        int bytesReceived = recv(clientSocket, ackData, sizeof(ackData), 0);
        if (bytesReceived > 0) {
            if (ackData[0] == '`') {
                continue;
            }
            ackData[bytesReceived] = '\0';
            int ackMessageType;
            sscanf(ackData, "2:%d", &ackMessageType);
            printf("Server: %s\n", ackData);
        }
    }

    // Close the client socket
    close(clientSocket);
    return 0;
}

