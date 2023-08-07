#include "commonutils.h"

/* Makes a socket file descriptor
* - a socket is basically a file in your computer
* - and socketFD is a description of that file
* 
* SOCK_STREAM - TCP connection
* AF_INET - internet protocol (IPv4)
* 0 - automatically assign a protocol (uses IP layer of Network Layer) 
*/
int getSocketTCPIPv4() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

/* sockaddr_in struct used for IPv4
* stores address information
* inet_pton() converts the ip string into a uint32_t and assign it
* htons() converts the number to proper endianness
* INADDR_ANY used by server to listen to any incoming addresses
*/
void setAddressProperties(struct sockaddr_in * address, int port, char * ip_address) {
    address->sin_port = htons(port);
    address->sin_family = AF_INET;

    if (strlen(ip_address) == 0)
        address->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET, ip_address, &address->sin_addr.s_addr);
}