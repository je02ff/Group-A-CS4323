//
// Created by jeff on 11/11/21.
//

#ifndef GROUP_A_CS4323_RODEN_H
#define GROUP_A_CS4323_RODEN_H

#include <stdbool.h>
#include "readDatabaseIntoArray.h"
#include "tcpStuff.h"


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

void deleteProduct(char *buffer, int* clientSock, int *deletedProducts);

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


#endif //GROUP_A_CS4323_RODEN_H
