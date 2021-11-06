#include <stdio.h>
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

#define PORT_NUM 8080
#define CLIENT_BACKLOG 25

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
    sAddress.sin_family = AF_INET;
    sAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    sAddress.sin_port = htons(PORT_NUM);
    if (bind(*sock,(struct sockaddr *)&(*sAddress), sizeof(*sAddress)) == -1) {
        printf("Failed to bind socket to port, exiting server\n");
        exit(1);
    } else {
        printf("Socket bound to port");
    }
}

void listenOnSocket(const int* sock, int backLogCount) {
    if(listen(*sock, backLogCount) == -1) {
        printf("Failed to listen, exiting server\n");
        exit(1);
    } else {
        printf("Listening through socket %d on port %d\n", *sock, PORT_NUM);
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

int main() {
    /* TCP Vars */
    int hSocket;
    int clientSock;
    struct sockaddr_in server = {0};
    struct sockaddr_in client = {0};
    pid_t pid;

    createSocket(&hSocket);                                             //Creating socket for server to communicate through
    bindSocket(&server, &hSocket);                                      // binding socket to address
    listenOnSocket(&hSocket, CLIENT_BACKLOG);                           //Listen for clients, maximum of 3 connections


    /*----Forking Clients----*/
    while (1) {
        if (acceptConnection(&clientSock, &hSocket, &client) == true) { //Accept client connection
            pid = fork();                                               //Fork client process
            if(pid == 0) {                                              //code for child(client) process
                close(hSocket);                                         //close hSocket, child uses clientSock

            } else {                                                    //I'm the parent, Do parent things
                close(clientSock);                                      //close childSocket, parent uses hSocket
                wait(NULL);                                     //prevent Zombies
            }
        }
    }
    return 0;
}



