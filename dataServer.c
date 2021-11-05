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

int main() {
    /* TCP Vars */
    int hSocket;
    int portNum = 90001;
    int clientLength;
    int newSock;
    struct sockaddr_in server = {0};
    struct sockaddr_in client = {0};
    pid_t pid;



    //Creating socket to communicate through
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket created\n");

    //Bind Socket to port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(portNum);
    bind(hSocket,(struct sockaddr *)&server, sizeof(server));
    printf("Socket binded\n");

    //Listen for clients, maximum of 3 connections
    listen(hSocket, 3);
    clientLength = sizeof(struct sockaddr_in);

    //Handle client traffic
    while (1) {
        //Accept client connection
        newSock = accept(hSocket, (struct sockaddr *)&client,(socklen_t*)&clientLength);
        if(newSock < 0) {
            exit(1);
        }
        printf("New Connection accepted\n");

        if((pid = fork()) == 0) {
            close(hSocket);
            while(1) {
                mainMenu(newSock);
                userInput = clientChoice(newSock);
                }
        } else { //I'm the parent, Do parent things

        }
    }
    close(newSock);
    return 0;
}



