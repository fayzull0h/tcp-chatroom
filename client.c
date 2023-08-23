#include "commonutils.h"
#include <ncurses.h>

#define NAME_SIZE 32

#define WINPUT_HEIGHT 6

struct thread_arg {
    WINDOW * win;
    int sock;
};

void * send_msg(void * arg);
void * recv_msg(void * arg);

char name[NAME_SIZE] = "DEFAULT";
pthread_mutex_t mutex;

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
    pthread_mutex_init(&mutex, NULL);

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
    wrefresh(input_win);
    wmove(input_win, 1, 1);
    refresh();

    char c;
    int x, y;
    int slen = 0;
    char mmm[BUF_SIZE] = {0};
    
    while (c = wgetch(input_win)) {
        if (c == '\n') 
            break;
        mmm[slen++] = c;
        getyx(input_win, y, x);
        wmove(input_win, y+1, x-1);
        waddch(input_win, c);
        wmove(input_win, y, x);

        if (slen == scrwidth-6) {
            int fx, fy;
            getyx(input_win, fy, fx);
            wmove(input_win, fy+1, 1);
        }
    }

    wmove(msgs_win, 1, 1);
    wprintw(msgs_win, "%s", mmm);
    refresh();
    wgetch(msgs_win);

    /* Create variables to send to threads */
    struct thread_arg send_th_arg = {0};
    send_th_arg.win = input_win;
    send_th_arg.sock = socketFD;

    /* Create threads for receiving and sending messages 
    pthread_create(&recv_th, NULL, recv_msg, (void *)&socketFD);
    pthread_create(&send_th, NULL, send_msg, (void *)&send_th_arg);
    //pthread_join(recv_th, &thread_return);
    pthread_join(send_th, &thread_return);*/

    close(socketFD);
    pthread_mutex_destroy(&mutex);

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
    struct thread_arg args = *((struct thread_arg *)arg);
    char msg[BUF_SIZE] = {0};
    char name_msg[NAME_SIZE + BUF_SIZE] = {0};
    int slen = 0;

    while (1) {
        msg[slen++] = wgetch(args.win);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            close(args.sock);
            exit(0);
        }
        sprintf(name_msg, "[%s]: %s", name, msg);
        write(args.sock, name_msg, strlen(name_msg));
    }
    return NULL;
}