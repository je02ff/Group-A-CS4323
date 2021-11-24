#ifndef GROUP_A_CLIENTCOPY_H
#define GROUP_A_CLIENTCOPY_H

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
void viewOrder();
void modifyOrder();
void viewBill();
void editInfo(int type, int clientSock);
void completeOrder();
void viewProducts();
void addProduct();
void addNewProduct();
void sellerMenu();
void deleteProduct();
void modifyQuantity();
void modifyPrice();


#endif //GROUP_A_CLIENTCOPY_H
