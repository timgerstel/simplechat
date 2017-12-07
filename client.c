/*
 * echoclient.c - An echo client
 */
/* $begin echoclientmain */
#include "csapp.h"

int main(int argc, char **argv) 
{
    int clientfd;
    char *host, *port, *username, buf[MAXLINE];
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

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        if(!(strcmp(buf, "quit\n"))){
            puts("Goodbye");
            exit(0);
        }
    	Rio_writen(clientfd, buf, strlen(buf));
    	Rio_readlineb(&rio, buf, MAXLINE);
    	Fputs(buf, stdout);
    }
    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
/* $end echoclientmain */
