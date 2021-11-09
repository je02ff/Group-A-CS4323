//
// Created by jeff on 11/9/21.
//

#ifndef GROUP_A_CS4323_READDATABASEINTOARRAY_H
#define GROUP_A_CS4323_READDATABASEINTOARRAY_H

#define maxRowsInDB 3000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


struct csvClientInfo {
    char uuid[5];
    char firstName[20];
    char lastName[20];
    char streetAddress[20];
    char city[20];
    char state[20];
    char zipCode[20];
};

struct csvProductInfo {
    char productId[5];
    char productName[20];
    char sellerId[5];
    char quantity[20];
    char price[20];
};

struct csvBillingInfo {
    char orderId[5];
    char customerId[5];
    char firstName[20];
    char lastName[20];
    char streetAddress[20];
    char city[20];
    char state[20];
    char zipCode[20];
    char totalOrderCost[20];
};

struct csvCustomerOrderInfo {
    char orderId[5];
    char productId[5];
    char quantityPurchased[20];
    char firstName[20];
    char lastName[20];
    char streetAddress[20];
    char city[20];
    char state[20];
    char zipCode[20];
    char totalPrice[20];  //quantityPurchased * itemPrice
};

/*--loads buyer or seller data into a table, keys off of dbFile name to determine which --*/
bool loadClientInfo(char* dbFile, struct csvClientInfo table[]);

/*--loads ProductInfo data into a table --*/
bool loadProductInfo(char* dbFile, struct csvProductInfo table[]);

/*--loads Customer Order data into a table --*/
bool loadCustomerOrderInfo(char* dbFile, struct csvCustomerOrderInfo table[]);

/*--loads Billing Info data into a table --*/
bool loadBillingInfo(char* dbFile, struct csvBillingInfo table[]);


#endif //GROUP_A_CS4323_READDATABASEINTOARRAY_H
