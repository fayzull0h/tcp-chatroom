#include "commonutils.h"

int main() {
    /* Get socket file descriptor */
    int servSocketFD = getSocketTCPIPv4();

    /* Specify address */
    struct sockaddr_in server_address;
    setAddressProperties(&server_address, 4444, "");

    /* Tell OS to bind port 4444 to this program */
    int result = bind(servSocketFD, (const struct sockaddr*)&server_address, sizeof(server_address));
    if (result == 0) printf("Socket bound succesfully!\n");

    /* Start listening for incoming connections 
    * Queue 10 connection requests before refusing new ones
    */
    result = listen(servSocketFD, 10);
    
    /* Variable for storing client's address */
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);

    /* Accept is a blocking func and returns socket file descriptor of connecting client */
    int clientSocketFD = accept(servSocketFD, (struct sockaddr *)&client_address, &client_address_size);
    if (clientSocketFD == -1) return 1;

    // Receive a message
    char buffer[1024];
    while(1) {
        ssize_t amount_received = recv(clientSocketFD, buffer, 1024, 0);

        if (amount_received > 0) {
            buffer[amount_received] = '\0';
            printf("Received: %s", buffer);
        } else break;
    }

    close(clientSocketFD);
    shutdown(servSocketFD, SHUT_RDWR);
    return 0;
}