#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

#define IP_ADDR "10.0.0.205"
#define MAX_CLIENTS 10
#define BUF_SIZE 1024
#define MINUTE 60

void handle_error(char * msg);

int getSocketTCPIPv4();

void setAddressProperties(struct sockaddr_in * address, int port, char * ip_address);

struct AcceptedConnection {
    int socketFD;
    struct sockaddr_in address;
};