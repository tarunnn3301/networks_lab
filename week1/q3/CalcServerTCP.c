
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <math.h>

#define MAX_MESSAGE_LENGTH 1024

// Check if a character is a valid arithmetic operator
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// Get precedence level of an arithmetic operator
int getPrecedence(char op) {
    if (op == '^')
        return 3;
    if (op == '*' || op == '/')
        return 2;
    if (op == '+' || op == '-')
        return 1;
    return 0;
}

// Check if an expression has balanced parentheses
bool isValidExpression(const char *expression) {
    int parentheses = 0;

    for (int i = 0; expression[i] != '\0'; i++) {
        if (expression[i] == '(') {
            parentheses++;
        } else if (expression[i] == ')') {
            if (parentheses == 0)
                return false;
            parentheses--;
        }
    }

    return parentheses == 0;
}

// Evaluate an arithmetic expression and return the result as a string
char *evaluateExpression(const char *expression) {
    double operand1, operand2;
    char operator;
    if (sscanf(expression, "%lf %c %lf", &operand1, &operator, &operand2) != 3) {
        return "Error: Invalid expression format";
    }

    double result;
    char *reply = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
    switch (operator) {
        case '+':
            result = operand1 + operand2;
            break;
        case '-':
            result = operand1 - operand2;
            break;
        case '*':
            result = operand1 * operand2;
            break;
        case '/':
            if (operand2 != 0) {
                result = operand1 / operand2;
            } else {
                return "Error: Division by zero";
            }
            break;
        case '^':
            result = pow(operand1, operand2);
            break;
        default:
            return "Error: Invalid operator";
    }

    sprintf(reply, "%lf", result);
    return reply;
}

// Thread function to handle client interactions
void *handleClient(void *arg) {
    int clientSocket = *((int *)arg);
    char data[MAX_MESSAGE_LENGTH];
    while (1) {
        int bytesReceived = recv(clientSocket, data, sizeof(data), 0);
        if (bytesReceived <= 0)
            break;

        data[bytesReceived] = '\0';
        int messageType;
        char encodedMessage[MAX_MESSAGE_LENGTH];

        sscanf(data, "%d:%[^\n]", &messageType, encodedMessage);

        printf("Received Message: %s\n", encodedMessage);

        if (messageType == 3) {
            break;
        }

        char *reply = evaluateExpression(encodedMessage);

        send(clientSocket, reply, strlen(reply), 0);
        free(reply);
    }

    close(clientSocket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server_Port_Number>\n", argv[0]);
        return 1;
    }

    int serverPort = atoi(argv[1]);
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error: Unable to create socket");
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error: Unable to bind to port");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Error: Unable to listen on socket");
        close(serverSocket);
        return 1;
    }

    printf("Server listening on port %d\n", serverPort);

    while (1) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);

        if (clientSocket == -1) {
            perror("Error: Unable to accept client connection");
            continue;
        }

        pthread_t threadId;
        if (pthread_create(&threadId, NULL, handleClient, &clientSocket) != 0) {
            perror("Error: Unable to create thread");
            close(clientSocket);
        }
    }

    close(serverSocket);
    return 0;
}
