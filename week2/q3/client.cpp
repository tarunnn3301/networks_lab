#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "encoder_decoder.cpp"

using namespace std;

const int MSG_LEN = 128;

void receiveMessages(int clientSocket) {
    char buffer[MSG_LEN];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            cout << "Server disconnected." << endl;
            // close(clientSocket);
            exit(0);
        }

        // Decode the received message
        string decodedMessage = base64_decode(buffer);

        // if(decodedMessage == "/exit"){
        //     close(clientSocket);
        // }
        if(decodedMessage[0] == '#'){
            decodedMessage.erase(0,1);
            cout << decodedMessage << endl;
            exit(0);
        }
        // if(decodedMessage.substr(0, decodedMessage.find(" ")) == "Server:"){
        {    
            cout << decodedMessage << endl;
            cout << "Enter message to send (or '/exit' to quit): "<<endl;
        }

        
        // Print the received message
        // cout << decodedMessage << endl;
    }
}

char* appendCharToCharArray(char* array, char a)
{
    size_t len = strlen(array);

    char* ret = new char[len+2];

    strcpy(ret, array);    
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <Server_IP_Address> <Server_Port_Number> <Username>" << endl;
        return 1;
    }

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(atoi(argv[2]));

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Connection failed");
        close(clientSocket);
        return 1;
    }
    else{
        cout<<"Connected to the server at "<<ntohs(serverAddress.sin_port)<<endl;
    }

    thread receiveThread(receiveMessages, clientSocket);
    receiveThread.detach();

    // do{
        // argv[3] += '#';
        argv[3] = appendCharToCharArray(argv[3], '#');
        string name = base64_encode(argv[3]);
        // string x = "#";
        // name += "#";
        cout<<name<<endl;
        send(clientSocket, name.c_str(), name.length(), 0);
    // }
    // while(0);

    while (true) {
        char message[MSG_LEN];
        cout << "Enter message to send (or '/exit' to quit): "<<endl;
        cin.getline(message, sizeof(message));

        if (string(message) == "/exit") {
            break;
        }

        // Encode the message
        string encodedMessage = base64_encode(message);

        // Send the encoded message to the server
        send(clientSocket, encodedMessage.c_str(), encodedMessage.length(), 0);

        // Receive acknowledgment from the server
        // char ackBuffer[MSG_LEN];
        // memset(ackBuffer, 0, sizeof(ackBuffer));
        // recv(clientSocket, ackBuffer, sizeof(ackBuffer), 0);

        // string decodedAck = base64_decode(ackBuffer);
        // cout << "Server Acknowledgment: " << decodedAck << endl;
    }

    // receiveThread.join();

    // Close the client socket
    close(clientSocket);
    return 0;
}
