//
// Created by jeff on 11/11/21.
//

#ifndef GROUP_A_CS4323_RODEN_H
#define GROUP_A_CS4323_RODEN_H

#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
#include "readDatabaseIntoArray.h"
#include "tcpStuff.h"

/*----Semaphores----*/
#define SEM_WRT_PRODUCT "/wrtProductInfo"
#define SEM_WRT_BILLING "/wrtBillingInfo"
#define SEM_WRT_SELLER "/wrtSellerInfo"
#define SEM_WRT_BUYER "/wrtCustomerInfo"
#define SEM_WRT_ORDER "/wrtCustomerOrderInfo"

#define SEM_READ_PRODUCT "/readProductInfo"
#define SEM_READ_BILLING "/readBillingInfo"
#define SEM_READ_SELLER "/readSellerInfo"
#define SEM_READ_BUYER "/readCustomerInfo"
#define SEM_READ_ORDER "/readCustomerOrderInfo"

//binary semaphores to lock writing priveledges
sem_t *wrtProductInfo;
sem_t *wrtBillingInfo;
sem_t *wrtCustomerInfo;
sem_t *wrtCustomerOrder;
sem_t *wrtSellerInfo;

//binary semaphores to lock modifying the number of readers reading(counters)
sem_t *ProductInfoMutex;
sem_t *BillingInfoMutex;
sem_t *CustomerInfoMutex;
sem_t *CustomerOrderMutex;
sem_t *SellerInfoMutex;

//list to keep track of products that have been deleted.
int deletedProducts[maxRowsInDB];
//global counting vars to keep track of readers
int buyerReaderCounter;
int sellerReaderCounter;
int billingReaderCounter;
int productReaderCounter;
int orderReaderCounter;

bool validateID(char* buffer);

int generateUID();

void extractCommand(char *buffer, char *command);

void writeNewClientData(char *buffer, int* clientSock);

void viewProducts(char *buffer, int* clientSock);

void extractBuyerOrder(char* buffer, struct itemOrder orderList[]);

bool validateBuyerOrder(struct itemOrder orderList[], struct csvProductInfo *pList);

void writeNewProductQuantityFromOrder(struct itemOrder *orderList, struct csvProductInfo *pList);

int writeOrderToBillingInfo(struct csvClientInfo buyerData, struct itemOrder *orderList, struct csvBillingInfo *billingList);

void writeOrderToCustomerOrder(int orderID, struct csvClientInfo buyerData, struct itemOrder orderList[], struct csvCustomerOrderInfo customerOrders[]);

int findBuyerInClientInfo(int buyerID, struct csvClientInfo buyerList[] );

void completeAnOrder(char *buffer, int* clientSock);

void buyerViewsOrder(char *buffer, int* clientSock);

void buyerViewsInfo(char *buffer, int* clientSock);

void addNewProduct(char *buffer, int* clientSock);

void deleteProduct(char *buffer, int* clientSock);

void modifyQuantity(char *buffer, int* clientSock);

void modifyPrice(char *buffer, int* clientSock);

void sellerViewsOrders(char *buffer, int* clientSock);

void clientEditsInfo(char *buffer, int* clientSock);

void sellerEditsName(char* buffer, int id);

void sellerEditsNumber(char* buffer, int id);

void sellerEditsAddress(char* buffer, int id);

void buyerEditsNumber(char* buffer, int id);

void buyerEditsName(char* buffer, int id);

void buyerEditsAddress(char* buffer, int id);

void buyerModifiesOrder(char *buffer, int* clientSock);

void readOrderDetails(char *buffer, int* clientSock);

bool validateOrderIsBuyers(char *buffer, int* clientSock);

void executeTaskOnServer(int clientSock);

#endif //GROUP_A_CS4323_RODEN_H
