// // #include <iostream>
// // #include <cstring>
// // #include <cstdlib>
// // #include <thread>
// // #include <vector>
// // #include <unistd.h>
// // #include <arpa/inet.h>
// // #include <sys/socket.h>
// // #include "encoder_decoder.cpp"

// // using namespace std;
// // const int MSG_LEN = 128;
// // vector<int> clientSockets;

// // void clientHandler(int clientSocket) {
// //     char buffer[MSG_LEN];
// //     while (true) {
// //         memset(buffer, 0, sizeof(buffer));
// //         int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

// //         if (bytesRead <= 0) {
// //             cout << "Client disconnected." << endl;
// //             break;
// //         }

// //         // Decode the received message
// //         string decodedMessage = base64_decode(buffer);

// //         // Print the decoded message
// //         cout << "Received: " << decodedMessage << endl;

// //         // Send acknowledgment to the client
// //         string ackMessage = decodedMessage;
// //         string encodedAck = base64_encode(ackMessage);
// //         send(clientSocket, encodedAck.c_str(), encodedAck.length(), 0);
// //     }

// //     close(clientSocket);
// // }

// // int main(int argc, char *argv[]) {
// //     if (argc != 2) {
// //         cerr << "Usage: " << argv[0] << " <Server_Port_Number>" << endl;
// //         return 1;
// //     }

// //     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
// //     if (serverSocket == -1) {
// //         perror("Socket creation failed");
// //         return 1;
// //     }

// //     sockaddr_in serverAddress, clientAddress;
// //     serverAddress.sin_family = AF_INET;
// //     serverAddress.sin_addr.s_addr = INADDR_ANY;
// //     serverAddress.sin_port = htons(atoi(argv[1]));

// //     if (::bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
// //         perror("Bind failed");
// //         close(serverSocket);
// //         return 1;
// //     }

// //     if (listen(serverSocket, 5) == -1) {
// //         perror("Listen failed");
// //         close(serverSocket);
// //         return 1;
// //     }

// //     cout << "Server listening on port " << argv[1] << endl;

// //     thread inputThread([&]() {
// //         string input;
// //         while (true) {
// //             getline(cin, input);
// //             for (int socket : clientSockets) {
// //                 string encodedMessage = base64_encode("Server: " + input);
// //                 send(socket, encodedMessage.c_str(), encodedMessage.length(), 0);
// //             }
// //         }
// //     });
// //     vector<thread> clientThreads;
// //     while (true) {
// //         socklen_t clientSize = sizeof(clientAddress);
// //         int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSize);

// //         if (clientSocket == -1) {
// //             perror("Accept failed");
// //             continue;
// //         }

// //         cout << "Client connected: " << inet_ntoa(clientAddress.sin_addr) << endl;
// //         clientThreads.emplace_back(clientHandler, clientSocket);
// //     }

// //     for (thread &t : clientThreads) {
// //         t.join();
// //     }

// //     inputThread.join();
// //     close(serverSocket);
// //     return 0;
// // }



#include <iostream>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <map>
#include "encoder_decoder.cpp"

using namespace std;
const int MSG_LEN = 128;

vector<int> clientSockets;
map<int, string> clients;

void clientHandler(int clientSocket) {
    char buffer[MSG_LEN];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            cout << clients[clientSocket] << " disconnected." << endl;
            // clientSockets.erase(remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end());
            clients.erase(clientSocket);
            break;
        }


        // Decode the received message
        string decodedMessage = base64_decode(buffer);

        if(decodedMessage.back() == '#'){
            decodedMessage.pop_back();
            bool client_present = false;
            for (int socket : clientSockets) {
                if(clients[socket] == decodedMessage){
                    string temp = base64_encode("#Client " + decodedMessage + " already exits");
                    send(clientSocket, temp.c_str(), temp.length(), 0);
                    client_present = true;
                    // close(socket);
                    // clients.erase(socket);
                    // clientSockets.pop_back();
                }
            }
            if(!client_present){
                clients[clientSocket] = decodedMessage;
                cout<<"Client Name : " << clients[clientSocket] << endl << endl;
            }
        }
        // Print the received message along with the sender's IP

        else if(decodedMessage.front() == '@'){
            cout << "Received from " << clients[clientSocket] << ": " << decodedMessage << endl;
            string receiver_name = decodedMessage.substr(0, decodedMessage.find(" "));
            receiver_name.erase(0, 1);
            decodedMessage.erase(0, decodedMessage.find(" "));
            bool found_client = false;
            for (int socket : clientSockets) {
                if(clients[socket] == receiver_name){
                    string encodedMessage = base64_encode(clients[clientSocket] + " :" + decodedMessage);
                    send(socket, encodedMessage.c_str(), encodedMessage.length(), 0);
                    found_client = true;
                    // cout << "Enter message to broadcast (or '/exit' to quit): "<<endl;
                }
            }
            if(!found_client){
                string x = base64_encode("Client " + receiver_name + " not found.");
                send(clientSocket, x.c_str(), x.length(), 0);
            }
            cout << "Sent to : " << receiver_name << endl << endl;
        }
        else{
            cout << "Received from "  << clients[clientSocket] <<": " << decodedMessage << endl;
            // thread sendmessage([&](){
                for (int socket : clientSockets) {
                    if (socket != clientSocket) {
                        string encodedMessage = base64_encode(decodedMessage);
                        send(socket, encodedMessage.c_str(), encodedMessage.length(), 0);
                    }
                }
            // });
            // thread send(send_messages, decodedMessage);
            // send_messages(decodedMessage);
            
            cout << "Broadcasted to Everyone" << endl << endl; 
        }
        
        // cout << "Enter message to broadcast (or '/exit' to quit): "<<endl;
        // Relay the message to all connected clients
        // for (int socket : clientSockets) {
        //     if (socket != clientSocket) {
        //         string encodedMessage = base64_encode(decodedMessage);
        //         send(socket, encodedMessage.c_str(), encodedMessage.length(), 0);
        //     }
        // }
    }

    close(clientSocket);
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <Server_Port_Number>" << endl;
        return 1;
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in serverAddress, clientAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(argv[1]));

    if (::bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Listen failed");
        close(serverSocket);
        return 1;
    }

    cout << "Server listening on port " << argv[1] << endl << endl;
    
    bool disconnected = false;
    thread inputThread([&]() {
        string input;
        while (true) {

            // int receiver;
            // cout << "Choose one socket id from following:"<<endl;
            // for(int i=0;i<clientSockets.size();i++){
            //     cout<<clientSockets[i]<<" ";
            // }
            // cin>>receiver;
            // cout << "Enter message to broadcast (or '/exit' to quit): "<<endl;
            getline(cin, input);
            if (input == "/exit") {
                cout << "\nServer is shutting down..." << endl;
                // Close all client sockets
                for (int socket : clientSockets) {
                    close(socket);
                    // cout << clients[socket] << " disconnected" << endl;
                }
                clientSockets.clear();
                // Close server socket and exit
                // close(serverSocket);
                // exit(0);
                disconnected = true;
                exit(0);
                break;

            }
            // for (int socket : clientSockets) {
            // // {
            //     string encodedMessage = base64_encode("Server: " + input);
            //     send(socket, encodedMessage.c_str(), encodedMessage.length(), 0);
            //     // cout << "Enter message to broadcast (or '/exit' to quit): "<<endl;
            // }
            
            // break;
        }
    });

    while (true) {
        if(disconnected){
            break;
        }
        socklen_t clientSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientSize);

        if (clientSocket == -1) {
            perror("Accept failed");
            continue;
        }

        cout << "Client connected: " << inet_ntoa(clientAddress.sin_addr) << endl;
        // int receiver;
        // cout << "Choose one socket id from following:"<<endl;
        // for(int i=0;i<clientSockets.size();i++){
        //     cout<<clientSockets[i]<<" ";
        // }
        // cin>>receiver;
        // cout << "Enter message to broadcast (or '/exit' to quit): "<<endl;
        clientSockets.push_back(clientSocket);

        thread(clientHandler, clientSocket).detach();
        // thread(send_messages, name1).detach();
    }

    
    // inputThread.join();
    close(serverSocket);
    return 0;
}







// new code



// #include <iostream>
// #include <cstring>
// #include <cstdlib>
// #include <unistd.h>
// #include <thread>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <vector>
// #include <map>
// #include "encoder_decoder.cpp"

// using namespace std;

// const int MSG_LEN = 128;

// // Structure to store client information
// struct ClientInfo {
//     int socket;
//     string name;
// };

// // Map to store connected clients
// map<int, ClientInfo> connectedClients;

// void receiveMessages(int clientSocket, int clientId) {
//     char buffer[MSG_LEN];
//     while (true) {
//         memset(buffer, 0, sizeof(buffer));
//         int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

//         if (bytesRead <= 0) {
//             cout << "Client " << connectedClients[clientId].name << " disconnected." << endl;
//             close(clientSocket);
//             connectedClients.erase(clientId);
//             break;
//         }

//         // Decode the received message
//         string decodedMessage = base64_decode(buffer);

//         // Handle the received message
//         cout << "Received from " << connectedClients[clientId].name << ": " << decodedMessage << endl;
//     }
// }

// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         cerr << "Usage: " << argv[0] << " <Server_Port_Number>" << endl;
//         return 1;
//     }

//     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket == -1) {
//         perror("Socket creation failed");
//         return 1;
//     }

//     sockaddr_in serverAddress;
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_addr.s_addr = INADDR_ANY;
//     serverAddress.sin_port = htons(atoi(argv[1]));

//     if (::bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
//         perror("Binding failed");
//         close(serverSocket);
//         return 1;
//     }

//     if (listen(serverSocket, 5) == -1) {
//         perror("Listening failed");
//         close(serverSocket);
//         return 1;
//     }

//     cout << "Server listening on port " << atoi(argv[1]) << "..." << endl;

//     int clientId = 0; // Unique identifier for each client

//     while (true) {
//         sockaddr_in clientAddress;
//         socklen_t clientAddrLen = sizeof(clientAddress);
//         int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);

//         if (clientSocket == -1) {
//             perror("Accepting connection failed");
//             continue;
//         }

//         // Get the client's IP address and port
//         char clientIP[INET_ADDRSTRLEN];
//         inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
//         int clientPort = ntohs(clientAddress.sin_port);

//         cout << "Client connected from " << clientIP << ":" << clientPort << endl;

//         // Prompt the client for their name
//         string clientName;
//         cout << "Enter your name: ";
//         cin >> clientName;

//         // Create a new client info entry
//         ClientInfo clientInfo;
//         clientInfo.socket = clientSocket;
//         clientInfo.name = clientName;
//         connectedClients[clientId] = clientInfo;

//         // Create a new thread to handle client messages
//         thread receiveThread(receiveMessages, clientSocket, clientId);
//         receiveThread.detach();

//         clientId++;
//     }

//     // Close the server socket (this code won't be reached)
//     close(serverSocket);
//     return 0;
// }
