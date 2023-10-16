
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <stdlib.h>

const int MAX_MSG_LENGTH = 1024;

// Check if a character is a valid arithmetic operator
bool isOperator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

// Get precedence level of an arithmetic operator
int getOperatorPrecedence(char op)
{
    if (op == '^')
        return 3;
    if (op == '*' || op == '/')
        return 2;
    if (op == '+' || op == '-')
        return 1;
    return 0;
}

// Check if an expression has balanced parentheses
bool isValidExpression(const char *exp)
{
    int parentheses = 0;

    for (int i = 0; exp[i] != '\0'; i++)
    {
        if (exp[i] == '(')
        {
            parentheses++;
        }
        else if (exp[i] == ')')
        {
            if (parentheses == 0)
                return false;
            parentheses--;
        }
    }

    return parentheses == 0;
}

// Evaluate an arithmetic expression and return the result as a dynamically allocated string
char *evaluateExpression(const char *expression)
{
    double a, b;
    char op;
    if (sscanf(expression, "%lf %c %lf", &a, &op, &b) != 3)
    {
        return strdup("Error: Invalid expression format"); // Duplicate the error message
    }

    double result;
    char resultStr[MAX_MSG_LENGTH]; // Use a buffer for the result string
    switch (op)
    {
    case '+':
        result = a + b;
        break;
    case '-':
        result = a - b;
        break;
    case '*':
        result = a * b;
        break;
    case '/':
        if (b != 0)
        {
            result = a / b;
        }
        else
        {
            return strdup("Error: Division by zero"); // Duplicate the error message
        }
        break;
    case '^':
        result = pow(a, b);
        break;
    default:
        return strdup("Error: Invalid operator"); // Duplicate the error message
    }

    snprintf(resultStr, MAX_MSG_LENGTH, "%lf", result);
    char *reply = strdup(resultStr); // Allocate memory and copy the result string
    return reply;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <Server_Port_Number>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1)
    {
        perror("Error: Unable to create socket");
        return 1;
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Error: Unable to bind to port");
        close(serverSocket);
        return 1;
    }

    printf("Server listening on port %d\n", port);

    char data[MAX_MSG_LENGTH];
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLen = sizeof(clientAddress);

    while (1)
    {
        int bytesReceived = recvfrom(serverSocket, data, sizeof(data), 0, (struct sockaddr *)&clientAddress, &clientAddressLen);
        if (bytesReceived <= 0)
            continue;

        data[bytesReceived] = '\0';

        char *decodedMessage = data;
        printf("Received Message from %s : %s\n", inet_ntoa(clientAddress.sin_addr), decodedMessage + 2);

        char *expression = decodedMessage + 2;

        if (isValidExpression(expression))
        {
            char *reply = evaluateExpression(expression);
            sendto(serverSocket, reply, strlen(reply), 0, (struct sockaddr *)&clientAddress, clientAddressLen);
            free(reply);
        }
        else
        {
            char *reply = "Error: Invalid expression";
            sendto(serverSocket, reply, strlen(reply), 0, (struct sockaddr *)&clientAddress, clientAddressLen);
        }
    }

    close(serverSocket);
    return 0;
}
