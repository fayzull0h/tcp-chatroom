#include "commonutils.h"

void receivePrintIncomingData(int socketFD) {
    char buffer[1024];

    while(1) {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);

        if (amountReceived > 0) {
            buffer[amountReceived] = '\0';
            printf("Received: %s", buffer);
        } else break;
    }
}

int main() {
    // Get socket file descriptor
    int socketFD = getSocketTCPIPv4();
    if (socketFD == -1) {
        printf("Error!\n");
        return 1;
    }
    
    // Specify address and port
    struct sockaddr_in address;
    setAddressProperties(&address, 4444, IP_ADDR); 

    // Attempt to connect to the server
    int result = connect(socketFD, (const struct sockaddr*)&address, sizeof(address));
    if (result == 0) printf("Connection was succesful!\n");
    else printf("Unsuccesful!\n");

    /* Listen for messages and print (on new thread) */
    pthread_t recvPrintID;
    pthread_create(&recvPrintID, NULL, (void *) receivePrintIncomingData, socketFD);

    /* Get input and send */
    char *line = NULL;
    size_t line_size = 0;
    printf("Type your message to send, type \"exit\" to close the application.\n");

    while(1) {
        ssize_t char_count = getline(&line, &line_size, stdin);

        if (char_count > 0 && strcmp("exit\n", line) == 0) break;

        ssize_t amount_sent = send(socketFD, line, char_count, 0);
    }

    close(socketFD);
    return 0;
}