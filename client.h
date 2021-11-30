// Header file for client.c functions
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define PORT 8081
#define BUFFERSIZE 10240

// menu functions
void initialMenu(int clientSock);
void buyerLogin(int clientSock);
void buyerMenu(int clientSock);
void sellerLogin(int clientSock);
void sellerMenu(int clientSock);
void userRegister(int type, int clientSock);
void IDLogin(int type, int clientSock);

// info handling functions
void makeOrder(int clientSock);
void viewOrder(int clientSock);
void modifyOrder(int clientSock);
void viewBill(int clientSock);
void editInfo(int type, int clientSock);
void completeOrder(int clientSock);
void viewProducts(int clientSock, int type);
void addProduct(int clientSock);
void addNewProduct(int clientSock);
void sellerMenu(int clientSock);
void deleteProduct(int clientSock);
void modifyQuantity(int clientSock);
void modifyPrice(int clientSock);