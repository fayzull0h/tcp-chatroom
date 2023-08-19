#include "commonutils.h"

void * handle_clients(void * arg);
void broadcast(char * msg, int slen);

int client_count = 0;
int client_sockfds[MAX_CLIENTS];

pthread_mutex_t mutex;

int main(int argc, char * argv[]) {
    int serv_sockfd, client_sockfd;
    struct sockaddr_in serv_addr = {0}, client_addr = {0};
    int client_addr_size = sizeof(client_addr);
    pthread_t tid;

    if (argc != 2) {
        printf("Usage: %s <PORT NUMBER>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);

    /* Get socket file descriptor */
    serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sockfd < 0) handle_error("[ERROR]: Could not create socket!");

    /* Specify address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    /* Tell OS to bind port 4444 to this program */
    if (bind(serv_sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        handle_error("[ERROR]: Could not bind!");
    printf("Program binded to port: %s\n", argv[1]);

    /**
     * Start listening for incoming connections 
     * Queue 10 connection requests before refusing new ones
     */
    if (listen(serv_sockfd, 10) != 0)
        handle_error("[ERROR]: Could not listen!");
    printf("Listening...\n");
    
    /* Get new connections,
    *  receive data and print (in a while loop) on a separate thread
    */
    while (1) {
        client_sockfd = accept(serv_sockfd, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_sockfd == -1) {
            printf("An accept() call failed, retrying...\n");
            continue;
        }

        pthread_mutex_lock(&mutex);
        client_sockfds[client_count++] = client_sockfd;
        pthread_mutex_unlock(&mutex);

        pthread_create(&tid, NULL, handle_clients, (void *)&client_sockfd);
        pthread_detach(tid);
        printf("Connected client IP: %s\n", inet_ntoa(client_addr.sin_addr));
    }

    pthread_mutex_destroy(&mutex);

    close(serv_sockfd);
    return 0;
}

void * handle_clients(void * arg) {
    int sock = *((int *)arg);
    int slen = 0;
    char msg[BUF_SIZE] = {0};

    while ((slen = read(sock, msg, BUF_SIZE)) != 0) {
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < client_count; i++) 
            write(client_sockfds[i], msg, slen);
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_count; i++) {
        if (client_sockfds[i] == sock) {
            for (int j = i; j < client_count - 1; j++)
                client_sockfds[j] = client_sockfds[j+1];
            break;
        }
    }
    client_count--;
    pthread_mutex_unlock(&mutex);
    close(sock);
    return NULL;
}

void broadcast(char * msg, int slen) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_count; i++) 
        write(client_sockfds[i], msg, slen);
    pthread_mutex_unlock(&mutex);
}