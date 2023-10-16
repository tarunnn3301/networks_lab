#include <iostream>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 12345;

using namespace std;

int main() {
    int sockfd;
    bool nak=false;
    string nakstring;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    char buffer[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    addr_size = sizeof(serverAddr);

    int totalFrames;
    std::cout << "Enter the total number of frames to be sent: ";

    std::cin >> totalFrames;




    int frameNumber = 1;

    while (frameNumber <= totalFrames) {
        char frame[256];
        snprintf(frame, sizeof(frame), "Frame %d", frameNumber);

       loop: sendto(sockfd, frame, strlen(frame), 0, (struct sockaddr*)&serverAddr, addr_size);
        std::cout << "Sending " << frame << std::endl;

        char ack[256];
    
        int ackSize = recvfrom(sockfd, ack, sizeof(ack), 0, (struct sockaddr*)&serverAddr, &addr_size);

        if (ackSize > 0) {
            ack[ackSize] = '\0';
            if (strcmp(ack, "ACK") == 0 && !nak) {
                std::cout << "Received ACK for Frame " << frameNumber << std::endl;
                frameNumber++;
            } if (strcmp(ack, "NAK") == 0 || nak) {
                std::cout << "Received NAK for Frame " << frameNumber << " Resending" << std::endl;
                nak=false;
                goto loop;
            }
        } else {
            std::cout << "Timeout: Resending Frame " << frameNumber << std::endl;
        }

        sleep(2);
    }

    close(sockfd);
    return 0;
}
