#include "commonutils.h"
#include <ncurses.h>

#define NAME_SIZE 32

#define WINPUT_HEIGHT 6

struct thread_arg {
    WINDOW * input_win;
    WINDOW * msgs_win;
    int sock;
    int scr_width;
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
    if (connect(socketFD, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
        handle_error("[ERROR]: Could not connect to the server!");

    /* NCURSES START */
    int scrwidth, scrheight;
    initscr();
    getmaxyx(stdscr, scrheight, scrwidth);
    cbreak();
    noecho();

    WINDOW * input_win = newwin(WINPUT_HEIGHT, scrwidth-4, scrheight-WINPUT_HEIGHT-1, 2);
    WINDOW * msgs_win = newwin(scrheight-WINPUT_HEIGHT-3, scrwidth-4, 1, 2);
    box(input_win, 0, 0);
    box(msgs_win, 0, 0);
    keypad(input_win, TRUE);
    
    wmove(stdscr, scrheight-WINPUT_HEIGHT-2, 3);
    printw("Enter message:");
    refresh();

    wrefresh(msgs_win);
    wrefresh(input_win);
    wmove(input_win, 1, 1);
    refresh();

    /* Create variables to send to threads */
    struct thread_arg args = {.input_win = input_win, .msgs_win = msgs_win, 
                              .sock = socketFD, .scr_width = scrwidth};

    /* Create threads for receiving and sending messages */
    pthread_create(&recv_th, NULL, recv_msg, (void *)&args);
    pthread_create(&send_th, NULL, send_msg, (void *)&args);
    pthread_join(send_th, &thread_return);
    pthread_cancel(recv_th);

    close(socketFD);
    pthread_mutex_destroy(&mutex);

    endwin();
    return 0;
}

void * recv_msg(void * arg) {
    struct thread_arg args = *((struct thread_arg *)arg);
    char name_msg[NAME_SIZE + BUF_SIZE] = {0};
    int slen = 0;
    int y = 1, x = 1;
    sleep(2);
    while ((slen = read(args.sock, name_msg, NAME_SIZE + BUF_SIZE - 1)) != 0) {
        pthread_mutex_lock(&mutex);
        if (slen == -1) {
            return (void *)-1;
            pthread_mutex_unlock(&mutex);
        }
        name_msg[slen] = '\0';
        wmove(args.msgs_win, y, x);
        for (int i = 0; i < slen; ++i) {
            waddch(args.msgs_win, name_msg[i]);
            if (++x == args.scr_width - 5) {
                x = 1;
                wmove(args.msgs_win, ++y, x);
            }
        }
        ++y, x = 1;
        box(args.msgs_win, 0, 0);
        wrefresh(args.msgs_win);
        wrefresh(args.input_win);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void * send_msg(void * arg) {
    struct thread_arg args = *((struct thread_arg *)arg);
    char msg[BUF_SIZE] = {0};
    char name_msg[NAME_SIZE + BUF_SIZE] = {0};
    keypad(args.input_win, 1);
    char c;
    int slen = 0;
    int y = 1, x = 1;

    while (c = wgetch(args.input_win)) {
        if (c == '\n') {
            msg[slen++] = c;
            pthread_mutex_lock(&mutex);
            msg[slen] = 0;
            if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
                pthread_mutex_unlock(&mutex);
                return NULL;
            }
            sprintf(name_msg, "[%s]: %s", name, msg);
            write(args.sock, name_msg, strlen(name_msg));
            wclear(args.input_win);
            box(args.input_win, 0, 0);
            wrefresh(args.input_win);
            pthread_mutex_unlock(&mutex);
            y = 1, x = 1;
            wmove(args.input_win, y, x);
            slen = 0;
        } else if (c == 7) {
            if (slen == 0) continue;
            getyx(args.input_win, y, x);
            mvwaddch(args.input_win, y, x-1, ' ');
            wmove(args.input_win, y, x-1);
            wrefresh(args.input_win);
            msg[--slen] = 0;
        } else {
            if (x == args.scr_width - 5) {
                x = 1;
                wmove(args.input_win, ++y, x);
            }
            waddch(args.input_win, c);
            msg[slen++] = c;
            ++x;
        }
    }
    return NULL;
}