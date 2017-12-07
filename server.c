/**Timothy Gerstel and Jonathan Azevedo
/* 
 * echoservert.c - A concurrent echo server using threads
 */
/* $begin echoservertmain */
#include "csapp.h"
#include <stdbool.h>

typedef struct{
    int fd;
    char* username;
    bool valid;
}Client_c;

int usercount = 0;
int lastremoved = 0;
Client_c connected[100];

void *thread(void *vargp);
void add_client(char*, int);
void remove_client(char*);
void send_message(char*, int connfd);

void echo(int connfd)
{
    int n;
    char buf[MAXLINE];
    char* username;
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);
    username = malloc(strlen(buf));
    strcpy(username, buf);
    *(username + strcspn(username, "\n")) = '\0';
    add_client(username, connfd);
    printf("@%s connected.\n", username);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        //printf("@%s: %s\n", username, buf);
        if(!strcmp(buf, "list-users\n")){
            int i;
            for(i = 0; i < usercount; i++){
                if(connected[i].valid){
                    char* user;
                    user = malloc(strlen(connected[i].username) + 1);
                    strcpy(user, connected[i].username);
                    strcat(user, "\n");
                    Rio_writen(connfd, user, strlen(user));
                }
            }
        }
        if(!strcmp(buf, "quit\n")){
            printf("@%s disconnected.\n", username);
            remove_client(username);
        }
        send_message(buf, connfd);
    }
}

void add_client(char* username, int connfd){
    if(!strcmp(username, "") || username == NULL){
        app_error("Invalid username");
    }
    connected[usercount].fd = connfd;
    connected[usercount].username = username;
    connected[usercount].valid = true;
    usercount++;
}

void remove_client(char* username){
    int i;
    for(i = 0; i < 100; i++){
        if(!strcmp(connected[i].username, username)){
            connected[i].valid = false;
            usercount--;
        }
        if(i < 99){
            if(!connected[i].valid && connected[i+1].valid){
                connected[i].valid = true;
                connected[i+1].valid = false;
                connected[i].username = connected[i+1].username;
                connected[i].fd = connected[i+1].fd;
            }
        }
    }
}

void send_message(char* buf, int connfd){
    char* cpy;
    char* username;
    char* message;
    int i;
    cpy = malloc(strlen(buf));
    username = malloc(strcspn(cpy, " "));
    message = malloc(strlen(cpy) - strlen(username));
    strcpy(cpy, buf);
    strncpy(username, cpy + 1, strcspn(cpy, " ") - 1);
    *(username + strcspn(username, "\n")) = '\0';
    strcpy(message, cpy + strlen(username) + 2);
    //printf("The message \"%s\" was directed to %s.\n", message, username);
    for(i = 0; i < usercount; i++){
        if(!strcmp(username, "broadcast")){
            Rio_writen(connected[i].fd, buf, strlen(buf));
        } else if(!strcmp(connected[i].username, username)){
            int clientfd = connected[i].fd;
            Rio_writen(clientfd, buf, strlen(buf));
            break;
        } else if (i == (usercount-1)){
            char* err = "User not found\n";
            Rio_writen(connfd, err, strlen(err));
        }
    }
}

int main(int argc, char **argv) 
{
    int listenfd, *connfdp, i;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid; 

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    listenfd = Open_listenfd(argv[1]);

    for(i = 0; i < 100; i++){
        connected[i].fd = 0;
        connected[i].username = "";
        connected[i].valid = false;
    }
    while (1) {
        clientlen=sizeof(struct sockaddr_storage);
    	connfdp = Malloc(sizeof(int)); //line:conc:echoservert:beginmalloc
    	*connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen); //line:conc:echoservert:endmalloc
    	Pthread_create(&tid, NULL, thread, connfdp);
    }
}

/* Thread routine */
void *thread(void *vargp) 
{  
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self()); //line:conc:echoservert:detach
    Free(vargp);                    //line:conc:echoservert:free
    echo(connfd);
    Close(connfd);
    return NULL;
}
/* $end echoservertmain */
