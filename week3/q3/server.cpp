#include <iostream>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno> // Include for error handling

const int PORT = 12345;
using namespace std;
int main() {
    int sockfd;
    bool nak=true;
    string nakstring;
    std::cout << "Do you want to send a NAK? yes/no ";
    std::cin >> nakstring;

    if (nakstring == "yes") {
        nak = true;
    } else {
        nak = false;
    }

    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    char buffer[1024];

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        close(sockfd); // Close the socket
        return 1;
    }

    addr_size = sizeof(clientAddr);

    while (true) {
        int recvBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &addr_size);

        if (recvBytes == -1 || nak) {
            perror("recvfrom");
            std::cout << "Sending NAK" << std::endl;
            nak=false;
            // Send a NAK as a response to the error
            sendto(sockfd, "NAK", 3, 0, (struct sockaddr*)&clientAddr, addr_size);
        } else if (recvBytes > 0) {
            buffer[recvBytes] = '\0';

            std::cout << "Received: " << buffer << std::endl;

            // Simulate ACK transmission
            sendto(sockfd, "ACK", 3, 0, (struct sockaddr*)&clientAddr, addr_size);
        }
    }

    close(sockfd);
    return 0;
}
