#include "commonutils.h"

struct AcceptedConnection clients[MAX_CLIENTS];
int clientCount = 0;

int getIncomingConnection(int servSocketFD, struct AcceptedConnection * acceptedSocket) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(servSocketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);

    acceptedSocket->address = clientAddress;
    acceptedSocket->socketFD = clientSocketFD;
    return clientSocketFD > 0;
}

void broadcast(char * msg) {
    for (int i = 0; i < clientCount; ++i) {
        if (clients[i].socketFD != -1) {
            send(clients[i].socketFD, msg, strlen(msg), 0);
        }
    }
}

void receivePrintIncomingData(int socketFD) {
    char buffer[1024];

    while(1) {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);

        if (amountReceived > 0) {
            buffer[amountReceived] = '\0';
            printf("Received: %s", buffer);

            broadcast(buffer);
        } else break;
    }
    close(socketFD);
}

int main() {
    /* Get socket file descriptor */
    int servSocketFD = getSocketTCPIPv4();

    /* Specify address */
    struct sockaddr_in serverAddress;
    setAddressProperties(&serverAddress, 4444, "");

    /* Tell OS to bind port 4444 to this program */
    int result = bind(servSocketFD, (const struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (result == 0) printf("Socket bound succesfully!\n");

    /* Start listening for incoming connections 
    * Queue 10 connection requests before refusing new ones
    */
    result = listen(servSocketFD, 10);
    
    /* Get new AcceptedConnection
    *  receive data and print (in a while loop) on a separate thread
    */
    while (1) {
        if (clientCount == 10) continue;

        struct AcceptedConnection newClient;
        int acceptResult = getIncomingConnection(servSocketFD, &newClient);
        clients[clientCount++] = newClient;

        pthread_t recvPrintID;
        pthread_create(&recvPrintID, NULL, (void *) receivePrintIncomingData, newClient.socketFD);
    }

    shutdown(servSocketFD, SHUT_RDWR);
    return 0;
}