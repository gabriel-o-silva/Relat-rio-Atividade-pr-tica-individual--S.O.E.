// user.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "common.h"

static mqd_t server_mqd = (mqd_t)-1;
static mqd_t user_mqd = (mqd_t)-1;
static char user_qname[64];
static pid_t mypid;
static volatile int running = 1;

void cleanup_and_exit(int code) {
    if (user_mqd != (mqd_t)-1) {
        mq_close(user_mqd);
        mq_unlink(user_qname);
    }
    if (server_mqd != (mqd_t)-1) mq_close(server_mqd);
    exit(code);
}

void sigint_handler(int sig) {
    (void)sig;
    running = 0;
}

void *reader_thread(void *arg) {
    (void)arg;
    chat_msg_t msg;
    while (running) {
        ssize_t r = mq_receive(user_mqd, (char*)&msg, sizeof(msg), NULL);
        if (r == -1) {
            // ignore or sleep briefly
            usleep(100000);
            continue;
        }
        // if server/system message
        if (msg.sender_pid == 0) {
            printf("%s\n", msg.text);
        } else {
            printf("[PID %d] %s\n", msg.sender_pid, msg.text);
        }
        fflush(stdout);
    }
    return NULL;
}

int main(void) {
    mypid = getpid();
    signal(SIGINT, sigint_handler);

    // open server queue for writing
    server_mqd = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_mqd == (mqd_t)-1) {
        perror("[USER] mq_open server queue");
        printf("Is the server running? Start the server first.\n");
        exit(1);
    }

    // create user queue name
    snprintf(user_qname, sizeof(user_qname), USER_QUEUE_NAME_FMT, (int)mypid);
    // unlink just in case
    mq_unlink(user_qname);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(chat_msg_t);
    attr.mq_curmsgs = 0;

    user_mqd = mq_open(user_qname, O_CREAT | O_RDONLY, 0666, &attr);
    if (user_mqd == (mqd_t)-1) {
        perror("[USER] mq_open user queue");
        exit(1);
    }

    // register with server
    chat_msg_t reg;
    memset(&reg,0,sizeof(reg));
    reg.type = MSG_REGISTER;
    reg.sender_pid = mypid;
    strncpy(reg.user_queue_name, user_qname, sizeof(reg.user_queue_name)-1);
    if (mq_send(server_mqd, (const char*)&reg, sizeof(reg), 0) == -1) {
        perror("[USER] mq_send register");
        cleanup_and_exit(1);
    }

    // spawn reader thread
    pthread_t th;
    if (pthread_create(&th, NULL, reader_thread, NULL) != 0) {
        perror("[USER] pthread_create");
        cleanup_and_exit(1);
    }

    // main loop: read stdin and send messages
    char line[512];
    printf("=== LocalIPC Chat (pid=%d). Type /quit to exit ===\n", (int)mypid);
    while (running && fgets(line, sizeof(line), stdin) != NULL) {
        // remove newline
        size_t L = strlen(line);
        if (L>0 && line[L-1]=='\n') line[L-1] = '\0';
        if (strcmp(line, "/quit") == 0) {
            running = 0;
            break;
        }
        if (line[0] == '\0') continue;

        chat_msg_t m;
        memset(&m,0,sizeof(m));
        m.type = MSG_TEXT;
        m.sender_pid = mypid;
        strncpy(m.text, line, sizeof(m.text)-1);

        if (mq_send(server_mqd, (const char*)&m, sizeof(m), 0) == -1) {
            perror("[USER] mq_send text");
        }
    }

    // send exit message to server
    chat_msg_t exitm;
    memset(&exitm,0,sizeof(exitm));
    exitm.type = MSG_EXIT;
    exitm.sender_pid = mypid;
    mq_send(server_mqd, (const char*)&exitm, sizeof(exitm), 0);

    // cleanup
    running = 0;
    pthread_cancel(th);
    pthread_join(th, NULL);
    cleanup_and_exit(0);
    return 0;
}
