/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"

void *io_thread(void *vargp);
void *receive_thread(void *vargp);

void prompt(){
    Fputs(">", stdout);
}

int main(int argc, char **argv) 
{
    int clientfd;
    char *host, *port, *username, buf[MAXLINE];
    pthread_t tid1, tid2;
    rio_t rio;

    if (argc != 4) {
	fprintf(stderr, "usage: %s <host> <port> <username>\n", argv[0]);
	exit(0);
    }
    host = argv[1];
    port = argv[2];
    username = argv[3];

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);
    strcpy(buf, username);
    strcat(buf, "\n");
    Rio_writen(clientfd, buf, strlen(buf));
    Pthread_create(&tid1, NULL, io_thread, (void *) &clientfd);
    Pthread_create(&tid2, NULL, receive_thread, (void *) &rio);
    Pthread_exit(NULL);
}

void *io_thread(void *vargp){
    int clientfd = *((int *)vargp);
    Pthread_detach(pthread_self()); //line:conc:echoservert:detach
    //Free(vargp);
    char buf[MAXLINE];
    prompt();
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        if(!strcmp(buf, "quit\n")){
            Rio_writen(clientfd, buf, strlen(buf));
            break;
        }
        Rio_writen(clientfd, buf, strlen(buf));
        prompt();
    }
    Close(clientfd);
    exit(0);
    return NULL;
}

void *receive_thread(void *vargp){
    rio_t rio = *((rio_t *)vargp);
    Pthread_detach(pthread_self()); //line:conc:echoservert:detach
    char buf[MAXLINE];
    while(1){
        Rio_readlineb(&rio, buf, MAXLINE);
        Fputs(buf, stdout);
    }
    return NULL;
}


/* $end echoclientmain */
