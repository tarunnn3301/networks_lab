
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>

const int MAX_MSG_LENGTH = 1024;

// Function to send a formatted message to the server
void sendMessageToServer(int clientSock, int msgType, const char *msg, const struct sockaddr_in *serverAddr)
{
    char fullMsg[MAX_MSG_LENGTH];
    snprintf(fullMsg, sizeof(fullMsg), "%d:%s", msgType, msg);

    // Use sendto to send messages in UDP
    sendto(clientSock, fullMsg, strlen(fullMsg), 0, (const struct sockaddr *)serverAddr, sizeof(*serverAddr));
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <Server_IP_Address> <Server_Port_Number>\n", argv[0]);
        return 1;
    }

    struct hostent *hostInfo = gethostbyname(argv[1]);
    if (hostInfo == NULL)
    {
        fprintf(stderr, "Error: %s is unavailable\n", argv[1]);
        exit(1);
    }

    struct in_addr *serverAddress = (struct in_addr *)hostInfo->h_addr;

    const char *serverIPAddress = inet_ntoa(*serverAddress);
    int serverPort = atoi(argv[2]);

    int clientSock = socket(AF_INET, SOCK_DGRAM, 0); // Use SOCK_DGRAM for UDP
    if (clientSock == -1)
    {
        fprintf(stderr, "Error creating socket\n");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIPAddress, &serverAddr.sin_addr) <= 0)
    {
        fprintf(stderr, "Error: Invalid address\n");
        close(clientSock);
        return 1;
    }

    char messageTypeStr[10];
    int messageType;
    char userMessage[MAX_MSG_LENGTH];
    char ackData[MAX_MSG_LENGTH];

    // Interaction loop with the server
    while (1)
    {
        printf("Enter Input Type (1: Expression, 3: Close): ");
        scanf("%s", messageTypeStr);

        if (strcmp(messageTypeStr, "1") != 0 && strcmp(messageTypeStr, "3") != 0)
        {
            printf("Error: Wrong Input type\n");
            continue;
        }

        messageType = atoi(messageTypeStr);

        if (messageType == 3)
        {
            sendMessageToServer(clientSock, messageType, "Closing communication", &serverAddr);
            break;
        }

        printf("Enter Expression (expression format: operand1 operator operand2): ");
        getchar(); // Consume newline left in input buffer
        fgets(userMessage, sizeof(userMessage), stdin);
        userMessage[strcspn(userMessage, "\n")] = '\0';

        sendMessageToServer(clientSock, messageType, userMessage, &serverAddr);

        struct sockaddr_in serverRespAddr;
        socklen_t serverRespAddrLen = sizeof(serverRespAddr);
        int bytesReceived = recvfrom(clientSock, ackData, sizeof(ackData), 0, (struct sockaddr *)&serverRespAddr, &serverRespAddrLen);

        if (bytesReceived > 0)
        {
            if (ackData[0] == '`')
            {
                continue;
            }
            ackData[bytesReceived] = '\0';
            int ackMessageType;
            sscanf(ackData, "2:%d", &ackMessageType);
            printf("Server: %s\n", ackData);
        }
    }

    close(clientSock);
    return 0;
}
