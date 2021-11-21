#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <semaphore.h>
#include "readDatabaseIntoArray.h"
#include "Roden.h"
#include <mqueue.h>
#include <sys/types.h>
#include <ctype.h>
#include <stddef.h>

#define SERVER_PORT_NUM 8081
#define CLIENT_BACKLOG 25
#define MSG_BUFFER_SIZE 10240
#define PARENT_QUEUE_NAME  "/sp-example-parent"
#define CHILD_QUEUE_NAME  "/sp-example-child"

#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 1024
#define MSG_BUFFER_SIZE_QUEUE MAX_MSG_SIZE + 10
#define MAX 9000000000 


/*---TCP Stuff---*/

void createSocket(int *sock);

void bindSocket(struct sockaddr_in *sAddress, const int *sock);

void listenOnSocket(const int* sock, int backLogCount);

bool acceptConnection(int* clientSock, const int* hSock, struct sockaddr_in* client);

void readSocket(const int* sock, char* buffer);

void writeSocket(const int* sock, char* buffer);

int main() {
    /* TCP Vars */
    int hSocket;
    int clientSock;
    struct sockaddr_in server = {0};
    struct sockaddr_in client = {0};
    char buffer[MSG_BUFFER_SIZE] = {0};
    pid_t pid;
    int fd[2];
    pipe(fd);
    int tempfork;
    char buffer2[10240];
    mqd_t messages;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;// The maximum number of messages that can be stored 	on the queue. 
    attr.mq_msgsize = MAX_MSG_SIZE;// The maximum size of each message on the given message queue. 
    attr.mq_curmsgs = 0;// This field represents the number of messages currently on the given queue.

    createSocket(&hSocket);                                             //Creating socket for server to communicate through
    bindSocket(&server, &hSocket);                                      // binding socket to address
    listenOnSocket(&hSocket, CLIENT_BACKLOG);                           //Listen for clients, maximum of 3 connections

  
    /*----Forking Clients---- */
    while (1) {
        if (acceptConnection(&clientSock, &hSocket, &client) == true) { //Accept incoming client connection
            pid = fork();                                         //Fork client process
            if(pid == 0) {                                              //code for child(client) process
                close(hSocket);
                close(fd[1]);
		if((messages = mq_open(PARENT_QUEUE_NAME, O_WRONLY)) == -1){
         	        perror("Child1: mq_open failed in child");
         	        exit(1);
      	        } 
                /*---Main Logic Loop: Driven by Client Commands ---*/
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
                while(1) {
                    readSocket(&clientSock, buffer);
                    if(mq_send(messages, buffer, strlen(buffer),0)==-1){perror("failed to send");}
                    memset(buffer,0, MSG_BUFFER_SIZE);
                    read(fd[0], &buffer, sizeof(buffer));
                    writeSocket(&clientSock, buffer);
                    memset(buffer,0, MSG_BUFFER_SIZE);
                    sleep(1);
                }
                close(fd[0]);
#pragma clang diagnostic pop
            } else if(pid > 0) {
            if((tempfork = fork()) == 0){                                       //I'm the parent, Do parent things
                close(clientSock);//close childSocket, parent uses hSocket
                close(fd[0]);
               if((messages = mq_open(PARENT_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1){
         	    perror("Child1: mq_open failed in parent");
         	    exit(1);
      	        }
      	        while(1){
      	           memset(buffer2,0, MSG_BUFFER_SIZE);
      	           if(mq_receive(messages, buffer2, MSG_BUFFER_SIZE_QUEUE, NULL) == -1){
      	              printf("failed to receive message");
      	           }
                   //send the string to the dataserver
                   
                   //change success to the result of the database server
                   write(fd[1], "SUCCESS", 8);    
      	        }
      	        close(fd[1]);                           
                wait(NULL); //reap children
              }                                    
            } else {
                printf("Fork failed!\n");
                exit(1);
            }
        }
    }
    return 0;
}

/*---TCP Stuff---*/

void createSocket(int *sock) {
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1) {
        printf("Failed to create socket, exiting server\n");
        exit(1);
    } else {
        printf("Socket created\n");
    }
}

void bindSocket(struct sockaddr_in *sAddress, const int *sock) {
    sAddress->sin_family = AF_INET;
    sAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    sAddress->sin_port = htons(SERVER_PORT_NUM);
    if (bind(*sock,(struct sockaddr *)&(*sAddress), sizeof(*sAddress)) == -1) {
        printf("Failed to bind socket to port, exiting server\n");
        exit(1);
    } else {
        printf("Socket bound to port\n");
    }
}

void listenOnSocket(const int* sock, int backLogCount) {
    if(listen(*sock, backLogCount) == -1) {
        printf("Failed to listen, exiting server\n");
        exit(1);
    } else {
        printf("Listening through socket %d on port %d\n", *sock, SERVER_PORT_NUM);
    }
}

bool acceptConnection(int* clientSock, const int* hSock, struct sockaddr_in* client) {
    int clientLength = sizeof(struct sockaddr_in);
    *clientSock = accept(*hSock, (struct sockaddr *)&(*client),(socklen_t*)&clientLength);
    if(*clientSock == -1) {
        printf("!!! Error connecting client !!!\n");
        return false;
    }
    printf("New Connection accepted...\n");
    return true;
}

void readSocket(const int* sock, char* buffer) {

    if(read(*sock, buffer, 10240) == -1) {
        printf("failed to read socket\n");
        exit(1);
    }
}

void writeSocket(const int* sock, char* buffer) {

    if (write(*sock, buffer, 10240) == -1) {
        printf("failed to write socket\n\n");
        exit(1);
    }

}
