#include "commonutils.h"
#include <ncurses.h>

#define NAME_SIZE 32

#define WINPUT_HEIGHT 6

void * send_msg(void * arg);
void * recv_msg(void * arg);

char name[NAME_SIZE] = "[DEFAULT]";

int main(int argc, char * argv[]) {
    /* Check input */
    if (argc != 4) {
        printf("Usage: %s <IP> <PORT NUMBER> <USERNAME>\n", argv[0]);
        handle_error("[ERROR]: Improper usage!");
    }
    strcpy(name, argv[3]);

    /* Set up variables for sockets and threads */
    int socketFD;
    struct sockaddr_in serv_addr;
    pthread_t recv_th, send_th;
    void * thread_return;

    /* Get socket file descriptor */
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD == -1) 
        handle_error("[ERROR]: Could not create client socket!");
    
    /* Specify address and port */
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));

    /* Attempt to connect to the server */
    if (connect(socketFD, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) printf("hi");
        //handle_error("[ERROR]: Could not connect to the server!");
    printf("Connected!\n");

    /* NCURSES START */
    int scrwidth, scrheight;
    initscr();
    getmaxyx(stdscr, scrheight, scrwidth);

    WINDOW * input_win = newwin(WINPUT_HEIGHT, scrwidth-4, scrheight-WINPUT_HEIGHT-1, 2);
    WINDOW * msgs_win = newwin(scrheight-WINPUT_HEIGHT-3, scrwidth-4, 1, 2);
    box(input_win, 0, 0);
    box(msgs_win, 0, 0);
    
    wmove(stdscr, scrheight-WINPUT_HEIGHT-2, 3);
    printw("Enter message:");

    refresh();
    wrefresh(msgs_win);
    wmove(input_win, 1, 1);
    wrefresh(input_win);
    getch();

    /* Listen for messages and print (on new thread) 
    pthread_create(&recv_th, NULL, recv_msg, (void *)&socketFD);
    pthread_create(&send_th, NULL, send_msg, (void *)&socketFD);
    pthread_join(recv_th, &thread_return);
    pthread_join(send_th, &thread_return);*/

    close(socketFD);

    endwin();
    return 0;
}

void * recv_msg(void * arg) {
    int sock = *((int *)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    int slen = 0;
    while (1) {
        slen = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        if (slen == -1)
            return (void *)-1;
        name_msg[slen] = 0;
        printf(name_msg);
    }
    return NULL;
}

void * send_msg(void * arg) {
    int sock = *((int *)arg);
    char msg[BUF_SIZE] = {0};
    char name_msg[NAME_SIZE + BUF_SIZE] = {0};

    while (1) {
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "[%s]: %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}