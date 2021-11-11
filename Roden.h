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

void writeNewClientData(char *buffer);

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

#endif //GROUP_A_CS4323_RODEN_H
