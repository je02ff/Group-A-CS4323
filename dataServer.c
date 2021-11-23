#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "readDatabaseIntoArray.h"
#include "tcpStuff.h"
#include "Roden.h"


#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"


/********************
Function: threadTask()
Purpose:  function that each thread in the thread pool will be executing.
Return Type: void
********************/
void * threadTask(void* ptrClientSocket) {
    int clientSock = *((int*)ptrClientSocket);
    free(ptrClientSocket);
    while(1) {
        executeTaskOnServer(clientSock);
    }
    return NULL;
}

int main() {
    /* TCP Vars */
    int hSocket;
    int clientSock;
    struct sockaddr_in server = {0};
    struct sockaddr_in client = {0};
    char buffer[MSG_BUFFER_SIZE];
    char command[25];

    createSocket(&hSocket);                                             //Creating socket for server to communicate through
    bindSocket(&server, &hSocket);                                      // binding socket to address
    listenOnSocket(&hSocket, CLIENT_BACKLOG);                           //Listen for clients, maximum of 3 connections


    //Handle client traffic
    while (1) {
        if (acceptConnection(&clientSock, &hSocket, &client) == true) {
            printf("...Client connected...\n");
        }

        pthread_t thread;
        int *ptrClientSock = malloc(sizeof(int));
        *ptrClientSock = clientSock;
        pthread_create(&thread, NULL, threadTask, ptrClientSock);

    }
    return 0;
}
#pragma clang diagnostic pop