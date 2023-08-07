#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    char *ip = "172.217.164.110";

    /* SocketFD - socket file descriptor
     * - a socket is basically a file in your computer
     * - and socketFD is a description of that file
     * 
     * SOCK_STREAM - TCP connection
     * AF_INET - internet protocol (IPv4)
     * 0 - automatically assign a protocol (uses IP layer of Network Layer) 
    */
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socketFD == -1) printf("Error!\n");

    /* sockaddr_in struct used for IPv4
     * stores address information
     * inet_pton() converts the ip string into a uint32_t and assign it
     * htons() converts the number to proper endianness
    */
    struct sockaddr_in address;
    address.sin_port = htons(80);
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr.s_addr);

    // Attempt to connect to a server
    int result = connect(socketFD, (const struct sockaddr*)&address, sizeof(address));
    if (result == 0) printf("Connection was succesful!\n");
    else printf("Unsuccesful!\n");

    return 0;
}