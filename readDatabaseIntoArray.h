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
    int uuid;
    char firstName[20];
    char lastName[20];
    char streetAddress[20];
    char city[20];
    char state[20];
    char zipCode[20];
};

struct csvProductInfo {
    int productId;
    char productName[20];
    int sellerId;
    int quantity;
    int price;
};

struct csvBillingInfo {
    int orderId;
    int customerId;
    char firstName[20];
    char lastName[20];
    char streetAddress[20];
    char city[20];
    char state[20];
    char zipCode[20];
    int totalOrderCost;
};

struct csvCustomerOrderInfo {
    int orderId;
    int productId;
    int quantityPurchased;
    char firstName[20];
    char lastName[20];
    char streetAddress[20];
    char city[20];
    char state[20];
    char zipCode[20];
    int totalPrice;  //quantityPurchased * itemPrice
};

struct itemOrder {
    int productID;
    int quantity;
    int itemCostPerUnit;
};

/*--loads buyer or seller data into a table, keys off of dbFile name to determine which --*/
bool loadClientInfo(char* dbFile, struct csvClientInfo table[]);

/*--loads ProductInfo data into a table --*/
bool loadProductInfo(struct csvProductInfo table[]);

/*--loads Customer Order data into a table --*/
bool loadCustomerOrderInfo(struct csvCustomerOrderInfo table[]);

/*--loads Billing Info data into a table --*/
bool loadBillingInfo(struct csvBillingInfo table[]);


#endif //GROUP_A_CS4323_READDATABASEINTOARRAY_H
