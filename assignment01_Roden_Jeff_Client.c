#include<stdio.h>
#include <stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <time.h>


/********************
Function: sleepTimer()
Purpose: Gets a random int between 1and 10 from the Server and then counts down
whoe lthe server sleeps during single player mode.
Return Type: void
********************/
void sleepTimer(int x) {
    
    for(int i=1; i <= x; i++) {
        printf("Sleep(%d)\n", i);
        sleep(1);
    }
    printf("\n");
}

/********************
Function: sendResponse()
Purpose: Gets input from user and sends to servr.
Return Type: void
********************/
void sendResponse(int hSocket) {
    char response[100] = {0};
    scanf("%s", response);
    send(hSocket, response, strlen(response), 0);
}

/********************
Function: fetchData()
Purpose: Responds to server commands to either sleep, quit, or respond with user input to the server.
Return Type: void
********************/
void fetchData(int hSocket, bool* hasQuit) {
    char dataBuffer[2000] = {0};
    recv(hSocket, dataBuffer, 1024, 0);

    if (strcmp(dataBuffer, "RESPOND") == 0) {
        sendResponse(hSocket);
    } else if(strcmp(dataBuffer, "SLEEP") == 0){
        int sleepTime;
        send(hSocket, "0", strlen("0"), 0);
        recv(hSocket, dataBuffer, 1024, 0);
        sleepTime = dataBuffer[0];
        sleepTimer(sleepTime);
        send(hSocket, "0", strlen("0"), 0);
    } else if(strcmp(dataBuffer, "QUIT") == 0) {
        send(hSocket, "0", strlen("0"), 0);
        printf("goodbye!");
        *hasQuit = true;
    } else {
        printf("%s",dataBuffer);
        send(hSocket, "0", strlen("0"), 0);

    }
}



int main() {
    int hSocket;
    char instruction[10] = {0};
    int portNum = 90001;
    char instruct;
    bool hasQuit = false;

    hSocket = socket(AF_INET, SOCK_STREAM, 0);  //Creating a Socket to communicate
  

    struct sockaddr_in server = {0};
    server.sin_addr.s_addr = inet_addr("10.203.72.5"); //csx8
    //server.sin_addr.s_addr = inet_addr("127.0.0.1"); //serv in vs
    //server.sin_addr.s_addr = inet_addr("10.0.2.15"); //client in vs
    server.sin_family = AF_INET;
    server.sin_port = htons(portNum);

    //connecting to server through hSocket through the server data
    while(1) {
        if(connect(hSocket,(struct  sockaddr *)&server, sizeof(struct sockaddr_in)) == 0) {
            printf("Connected to server\n");
            break;
        }
    }
        
    while(!hasQuit) {
          fetchData(hSocket, &hasQuit);
          
    }
    return 0;
}