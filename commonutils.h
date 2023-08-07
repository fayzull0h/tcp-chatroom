#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define IP_ADDR "10.0.0.205"

int getSocketTCPIPv4();

void setAddressProperties(struct sockaddr_in * address, int port, char * ip_address);