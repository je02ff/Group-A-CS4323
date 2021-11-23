#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc50-cpp"
#pragma ide diagnostic ignored "cert-msc51-cpp"
#pragma ide diagnostic ignored "cert-err34-c"
#include <stdio.h>
#include <strings.h>
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
#include <signal.h>
#include <assert.h>
#include <semaphore.h>
#include "Roden.h"
#include "tcpStuff.h"
#include "readDatabaseIntoArray.h"
#include "JacobWilson.h"

int deletedProducts[maxRowsInDB] = {0};

/*---TCP Stuff---*/

void createSocket(int *sock) {
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1) {
        printf("Failed to create socket, exiting server\n");
        exit(1);
    } else {
        printf("Socket created\n");
    }
}

void bindSocket(struct sockaddr_in *sAddress, const int *sock) {
    sAddress->sin_family = AF_INET;
    sAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    sAddress->sin_port = htons(DATA_SERVER_PORT_NUM);
    if (bind(*sock,(struct sockaddr *)&(*sAddress), sizeof(*sAddress)) == -1) {
        printf("Failed to bind socket to port, exiting server\n");
        exit(1);
    } else {
        printf("Socket bound to port\n");
    }
}

void listenOnSocket(const int* sock, int backLogCount) {
    if(listen(*sock, backLogCount) == -1) {
        printf("Failed to listen, exiting server\n");
        exit(1);
    } else {
        printf("Listening through socket %d on port %d\n", *sock, DATA_SERVER_PORT_NUM);
    }
}

bool acceptConnection(int* clientSock, const int* hSock, struct sockaddr_in* client) {
    int clientLength = sizeof(struct sockaddr_in);
    *clientSock = accept(*hSock, (struct sockaddr *)&(*client),(socklen_t*)&clientLength);
    if(*clientSock == -1) {
        printf("!!! Error connecting client !!!\n");
        return false;
    }
    printf("New Connection accepted...\n");
    return true;
}

void readSocket(const int* sock, char* buffer) {

    if(read(*sock, buffer, 10240) == -1) {
        printf("failed to read socket\n");
        exit(1);
    }
}

void writeSocket(const int* sock, char* buffer) {

    if (write(*sock, buffer, 10240) == -1) {
        printf("failed to write socket\n\n");
        exit(1);
    }

}

/*---Database Table Loaders---*/

bool loadClientInfo(char* dbFile, struct csvClientInfo table[]) {
    char buffer[200] ;
    char *record,*line;
    int rowCount = 0;

    FILE *fstream = fopen(dbFile,"r");

    if(fstream == NULL) {
        return false;
    }

    while((line=fgets(buffer,sizeof(buffer),fstream))!=NULL) {
        record = strtok(line,",");
        while(record != NULL) {
            table[rowCount].uuid = atoi(record);
            strcpy(table[rowCount].firstName, strtok(NULL,","));
            strcpy(table[rowCount].lastName, strtok(NULL,","));
            strcpy(table[rowCount].phoneNumber


                   , strtok(NULL,","));
            strcpy(table[rowCount].streetAddress, strtok(NULL,","));
            strcpy(table[rowCount].city, strtok(NULL,","));
            strcpy(table[rowCount].state, strtok(NULL,","));
            strcpy(table[rowCount].zipCode, strtok(NULL,","));
            record = NULL;
            rowCount++;
        }
    }
    return true;
}

bool loadProductInfo(struct csvProductInfo table[]) {
    char buffer[200] ;
    char *record,*line;
    int rowCount = 0;

    FILE *fstream = fopen("ProductInfo.txt","r");

    if(fstream == NULL) {
        return false;
    }

    while((line=fgets(buffer,sizeof(buffer),fstream))!=NULL) {
        record = strtok(line,",");
        while(record != NULL) {
            table[rowCount].productId = atoi(record);
            strcpy(table[rowCount].productName, strtok(NULL,","));
            table[rowCount].sellerId = atoi(strtok(NULL,","));
            table[rowCount].quantity =  atoi(strtok(NULL,","));
            table[rowCount].price = atoi(strtok(NULL,","));
            record = NULL;
            rowCount++;
        }
    }
    return true;
}

bool loadCustomerOrderInfo(struct csvCustomerOrderInfo table[]) {
    char buffer[200] ;
    char *record,*line;
    int rowCount = 0;

    FILE *fstream = fopen("CustomerOrder.txt","r");

    if(fstream == NULL) {
        return false;
    }

    while((line=fgets(buffer,sizeof(buffer),fstream))!=NULL) {
        record = strtok(line,",");
        while(record != NULL) {
            table[rowCount].orderId = atoi(record);
            table[rowCount].productId = atoi(strtok(NULL,","));
            table[rowCount].quantityPurchased = atoi(strtok(NULL,","));
            strcpy(table[rowCount].firstName, strtok(NULL,","));
            strcpy(table[rowCount].lastName, strtok(NULL,","));
            strcpy(table[rowCount].streetAddress, strtok(NULL,","));
            strcpy(table[rowCount].city, strtok(NULL,","));
            strcpy(table[rowCount].state, strtok(NULL,","));
            strcpy(table[rowCount].zipCode, strtok(NULL,","));
            table[rowCount].totalPrice = atoi(strtok(NULL,","));
            record = NULL;
            rowCount++;
        }
    }
    return true;
}

bool loadBillingInfo(struct csvBillingInfo table[]) {
    char buffer[200] ;
    char *record,*line;
    int rowCount = 0;

    FILE *fstream = fopen("BillingInfo.txt","r");
    if(fstream == NULL) {
        return false;
    }

    while((line=fgets(buffer,sizeof(buffer),fstream))!=NULL) {
        record = strtok(line,",");
        while(record != NULL) {
            table[rowCount].orderId = atoi(record);
            table[rowCount].customerId = atoi(strtok(NULL,","));
            strcpy(table[rowCount].firstName, strtok(NULL,","));
            strcpy(table[rowCount].lastName, strtok(NULL,","));
            strcpy(table[rowCount].streetAddress, strtok(NULL,","));
            strcpy(table[rowCount].city, strtok(NULL,","));
            strcpy(table[rowCount].state, strtok(NULL,","));
            strcpy(table[rowCount].zipCode, strtok(NULL,","));
            table[rowCount].totalOrderCost = atoi(strtok(NULL,","));
            record = NULL;
            rowCount++;
        }
    }
    return true;
}

/*---Data Server Commands---*/

int generateUID() {

    srand(time(NULL));
    int idNum = rand() % 9000 + 1000;

    return idNum;
}

void extractCommand(char *buffer, char *command) {
    char* ptr;
    ptr = strtok(buffer,",");

    if(ptr) {
        sprintf(command, "%s", ptr);
    }
    ptr = strtok(NULL, "");

    if(ptr)
        sprintf(buffer, "%s\n", ptr);
}

bool validateID(char* buffer) {
    /*Required Buffer string: "buyerID,[BUYER]"
                        or "sellerID,[SELLER]"
                        or "productID,[PRODUCT]"
                        or "orderID,[BILLING]"
                        or "productID,[ORDER]"*/
    int idToValidate;
    char dbName[20];
    char* point;
    int rowCount = 0;

    //extract id
    point = strtok(buffer,",");
    idToValidate = atoi(point);
    strcpy(dbName, strtok(NULL, ","));

    if (strstr(dbName, "[BUYER]") != NULL) {
        struct csvClientInfo buyerList[maxRowsInDB];
        if (loadClientInfo("CustomerInfo.txt", buyerList)) {
            while (buyerList[rowCount].uuid != 0) {
                if (buyerList[rowCount].uuid == idToValidate) {return true; }
                rowCount++;
            }
        }
        return false; // returns false if a problem or if ID is not present

    } else if (strstr(dbName, "[SELLER]") != NULL) {
        struct csvClientInfo sellerList[maxRowsInDB];
        if (loadClientInfo("SellerInfo.txt", sellerList)) {
            while (sellerList[rowCount].uuid != 0) {
                if (sellerList[rowCount].uuid == idToValidate) { return true; }
                rowCount++;
            }
        }
        return false;

    } else if (strstr(dbName, "[PRODUCT]") != NULL) {
        struct csvProductInfo productList[maxRowsInDB];
        if (loadProductInfo(productList)) {
            while (productList[rowCount].productId != 0) {
                if (productList[rowCount].productId == idToValidate) { return true; }
                rowCount++;
            }
        }
        return false;

    } else if (strstr(dbName, "[BILLING]") != NULL) {
        struct csvBillingInfo billingList[maxRowsInDB];
        if (loadBillingInfo(billingList)) {
            while (billingList[rowCount].orderId != 0) {
                if (billingList[rowCount].orderId == idToValidate) { return true; }
                rowCount++;
            }
        }
    } else if (strstr(dbName, "[ORDER]") != NULL) {
        struct csvCustomerOrderInfo orderData[maxRowsInDB];
        if (loadCustomerOrderInfo(orderData)) {
            while (orderData[rowCount].orderId != 0) {
                if (orderData[rowCount].productId == idToValidate) { return true; }
                rowCount++;
            }
        }
    return false;

    } else return false;
}

void writeNewClientData(char *buffer, int* clientSock) {
    int uuid;
    char stringID[MSG_BUFFER_SIZE] = {0};
    char clientType[25] = {0};
    char firstName[25] = {0};
    char lastName[25] = {0};
    char phoneNumber[25] = {0};
    char streetAddress[25] = {0};
    char city[25] = {0};
    char state[25] = {0};
    char zipCode[25] = {0};
    char* ptr;
    struct csvClientInfo clientDB[maxRowsInDB];

    //extract clientType
    ptr = strtok(buffer,",");
    sprintf(clientType, "%s", ptr);

    //extract firstName
    ptr = strtok(NULL, ",");
    sprintf(firstName, "%s", ptr);

    //extract lastName
    ptr = strtok(NULL, ",");
    sprintf(lastName, "%s", ptr);

    //extract phoneNumber
    ptr = strtok(NULL, ",");
    sprintf(phoneNumber, "%s", ptr);

    //extract streetAddress
    ptr = strtok(NULL, ",");
    sprintf(streetAddress, "%s", ptr);

    //extract city
    ptr = strtok(NULL, ",");
    sprintf(city, "%s", ptr);

    //extract state
    ptr = strtok(NULL, ",");
    sprintf(state, "%s", ptr);

    //extract zipCode
    ptr = strtok(NULL, ",");
    sprintf(zipCode, "%s", ptr);

    if (strstr(clientType, "[BUYER]") != NULL) {
        int lastOccupiedRow = 0;

        uuid = generateUID();
        loadClientInfo("CustomerInfo.txt", clientDB);
        while (clientDB[lastOccupiedRow].uuid != 0) {
            lastOccupiedRow++;
        }
        clientDB[lastOccupiedRow+1].uuid = uuid;
        strcpy(clientDB[lastOccupiedRow+1].firstName, firstName);
        strcpy(clientDB[lastOccupiedRow+1].lastName, lastName);
        strcpy(clientDB[lastOccupiedRow+1].streetAddress, streetAddress);
        strcpy(clientDB[lastOccupiedRow+1].city, city);
        strcpy(clientDB[lastOccupiedRow+1].state, state);
        strcpy(clientDB[lastOccupiedRow+1].zipCode, zipCode);

        sprintf(stringID, "%d", uuid);
        writeSocket(clientSock,stringID);

        writeBackClientStruct("CustomerInfo.txt", clientDB);
    } else {
        int lastOccupiedRow = 0;

        uuid = generateUID();
        loadClientInfo("SellerInfo.txt", clientDB);

        while (clientDB[lastOccupiedRow].uuid != 0) {
            lastOccupiedRow++;
        }

        clientDB[lastOccupiedRow+1].uuid = uuid;
        strcpy(clientDB[lastOccupiedRow+1].firstName, firstName);
        strcpy(clientDB[lastOccupiedRow+1].lastName, lastName);
        strcpy(clientDB[lastOccupiedRow+1].streetAddress, streetAddress);
        strcpy(clientDB[lastOccupiedRow+1].city, city);
        strcpy(clientDB[lastOccupiedRow+1].state, state);
        strcpy(clientDB[lastOccupiedRow+1].zipCode, zipCode);

        sprintf(stringID, "%d", uuid);
        writeSocket(clientSock,stringID);

        writeBackClientStruct("SellerInfo.txt", clientDB);
    }
}

void viewProducts(char *buffer, int* clientSock) {
    //REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[BUYER],"
    //OR REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[SELLER],uuid,"
    int rowCount = 0;
    char dataToSendClient[MSG_BUFFER_SIZE] = {0};
    char numsToString[20] = {0};
    char clientType[25] = {0};
    char* ptr;

    //extract clientType
    ptr = strtok(buffer,",");
    sprintf(clientType, "%s", ptr);

    if (strstr(clientType, "[BUYER]") != NULL) {
        //SEND ALL: ProductIDs, Product Names, Quantitys, Price/Units*/
        struct csvProductInfo productList[maxRowsInDB];
        if (loadProductInfo(productList)) {
            while(productList[rowCount].productId != 0) {
                sprintf(numsToString,"%d", productList[rowCount].productId);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");

                strcat(dataToSendClient, productList[rowCount].productName);
                strcat(dataToSendClient, ",");

                bzero(numsToString,20);
                sprintf(numsToString,"%d", productList[rowCount].quantity);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");

                bzero(numsToString,20);
                sprintf(numsToString,"%d", productList[rowCount].price);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",\n");
                rowCount++;
            }
            writeSocket(clientSock,dataToSendClient);
        }
    } else {                                            //IF Seller
        /*READ ProductInfo.txt
        SEND FILTERED to Seller's ID:  ProductIDs, Product Names, Quantitys, Price/Units*/

        //extract seller's ID

        int requestingSellerID = atoi(strtok(buffer,","));
        struct csvProductInfo productList[maxRowsInDB];
        if (loadProductInfo(productList)) {
            while(productList[rowCount].productId != 0 && productList[rowCount].sellerId != requestingSellerID) {
                sprintf(numsToString,"%d", productList[rowCount].productId);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");

                strcat(dataToSendClient, productList[rowCount].productName);
                strcat(dataToSendClient, ",");

                bzero(numsToString,20);
                sprintf(numsToString,"%d", productList[rowCount].quantity);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");

                bzero(numsToString,20);
                sprintf(numsToString,"%d", productList[rowCount].price);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",\n");
                bzero(numsToString,20);
                rowCount++;
            }

            writeSocket(clientSock,dataToSendClient);
        }
    }
}

void extractBuyerOrder(char* buffer, struct itemOrder orderList[]) {
    char* ptr;
    int rowOrder = 0;

    ptr = strtok(buffer, ",");
    while(atoi(ptr) != 0) {

        orderList[rowOrder].productID = atoi(ptr);
        ptr = strtok(NULL, ",");
        orderList[rowOrder].quantity = atoi(ptr);
        ptr = strtok(NULL, ",");
        orderList[rowOrder].itemCostPerUnit = atoi(ptr);
        ptr = strtok(NULL, ",");
        rowOrder++;
    }

}

bool validateBuyerOrder(struct itemOrder orderList[], struct csvProductInfo *pList) {

    bool idFound = false;
    int rowsInOrderList = 0;
    int rowsInProductList = 0;

    while (pList[rowsInProductList].productId != 0) {
        rowsInProductList++;
    }
    while (orderList[rowsInOrderList].productID != 0) {
        rowsInOrderList++;
    }

    for(int i = 0; i <= rowsInOrderList; i++) {
        for(int j = 0; j <= rowsInProductList; j++) {
            if(orderList[i].productID == pList[j].productId) {
                idFound = true;
                if (orderList[i].quantity > pList[j].quantity) return false;
            }
        }
        if(!idFound) return false;
        else idFound = false;
    }

    return true;
}

void writeNewProductQuantityFromOrder(struct itemOrder *orderList, struct csvProductInfo *pList) {
    int rowsInOrderList = 0;
    int rowCounter = 0;

    while (orderList[rowsInOrderList].productID != 0) {
        rowsInOrderList++;
    }

    for (int i = 0; i < rowsInOrderList; i++) {
        while(orderList[i].productID != pList[rowCounter].productId) {
            rowCounter++;
        }
        pList[rowCounter].quantity -= orderList[i].quantity;
        rowCounter = 0;
    }

    writeBackProductStruct(pList);
}

int writeOrderToBillingInfo(struct csvClientInfo buyerData, struct itemOrder *orderList, struct csvBillingInfo *billingList) {
    int rowToWriteRecord = 0;
    int totalOrderCost = 0;
    int productListRow = 0;

    //Find last occupied record in BillingInfo.txt
    while(billingList[rowToWriteRecord].orderId != 0) {
        rowToWriteRecord++;
    }
    //Total the cost of all the ordered products
    while(orderList[productListRow].productID != 0) {
        totalOrderCost += (orderList[productListRow].quantity * orderList[productListRow].itemCostPerUnit);
        productListRow++;
    }

    //Add thew new order to table
    billingList[rowToWriteRecord].orderId = generateUID();
    billingList[rowToWriteRecord].customerId = buyerData.uuid;
    strcpy(billingList[rowToWriteRecord].firstName, buyerData.firstName);
    strcpy(billingList[rowToWriteRecord].lastName, buyerData.lastName);
    strcpy(billingList[rowToWriteRecord].streetAddress, buyerData.streetAddress);
    strcpy(billingList[rowToWriteRecord].city, buyerData.city);
    strcpy(billingList[rowToWriteRecord].state, buyerData.state);
    strcpy(billingList[rowToWriteRecord].zipCode, buyerData.zipCode);
    billingList[rowToWriteRecord].totalOrderCost = totalOrderCost;

    writeBackBillingStruct(billingList);

    return billingList[rowToWriteRecord].orderId;
}

void writeOrderToCustomerOrder(int orderID, struct csvClientInfo buyerData, struct itemOrder orderList[], struct csvCustomerOrderInfo customerOrders[]) {
    int lastRecordRow = 0;
    int rowsInOrderList = 0;

    while(customerOrders[lastRecordRow].orderId != 0) {
        lastRecordRow++;
    }
    while (orderList[rowsInOrderList].productID != 0) {
        rowsInOrderList++;
    }
    for(int i = 0; i < rowsInOrderList; i++) {
        customerOrders[lastRecordRow].orderId = orderID;
        customerOrders[lastRecordRow].productId = orderList[i].productID;
        customerOrders[lastRecordRow].quantityPurchased = orderList[i].quantity;
        strcpy(customerOrders[lastRecordRow].firstName, buyerData.firstName);
        strcpy(customerOrders[lastRecordRow].lastName, buyerData.lastName);
        strcpy(customerOrders[lastRecordRow].streetAddress, buyerData.streetAddress);
        strcpy(customerOrders[lastRecordRow].city, buyerData.city);
        strcpy(customerOrders[lastRecordRow].state, buyerData.state);
        strcpy(customerOrders[lastRecordRow].zipCode, buyerData.zipCode);
        customerOrders[lastRecordRow].totalPrice = orderList[i].quantity * orderList[i].itemCostPerUnit;
        lastRecordRow++;
    }

    //write customerOrders to text file
    writeBackCustomerOrderStruct(customerOrders);

}

int findBuyerInClientInfo(int buyerID, struct csvClientInfo buyerList[] ) {
    int rowCount = 0;
    while(buyerID != buyerList[rowCount].uuid) {
        rowCount++;
    }

    return rowCount;
}

void completeAnOrder(char *buffer, int* clientSock) {
    //Required Buffer String: "[COMPLETE_ORDER],int buyerID,int productID,int quantityOrdered,int pricePerUnit, int buyerID,int productID,int quantityOrdered,int pricePerUnit, ...,"
    int buyerID;
    struct itemOrder orderList[100];
    char* ptr;
    struct csvProductInfo productList[maxRowsInDB];

    buyerID = atoi(strtok(buffer,","));

    ptr = strtok(NULL, "");
    sprintf(buffer, "%s\n", ptr);

    extractBuyerOrder(buffer, orderList);
    loadProductInfo(productList);

    if(validateBuyerOrder(orderList, productList)) {
        int buyerRecordRow;
        int orderID;
        struct csvClientInfo buyerList[maxRowsInDB];
        struct csvBillingInfo billingInfo[maxRowsInDB];
        struct csvCustomerOrderInfo customerOrderInfo[maxRowsInDB];


        loadClientInfo("CustomerInfo.txt", buyerList);
        loadBillingInfo(billingInfo);
        loadCustomerOrderInfo(customerOrderInfo);

        buyerRecordRow = findBuyerInClientInfo(buyerID, buyerList);

        writeNewProductQuantityFromOrder(orderList, productList);
        orderID = writeOrderToBillingInfo(buyerList[buyerRecordRow], orderList, billingInfo);
        writeOrderToCustomerOrder(orderID, buyerList[buyerRecordRow], orderList,customerOrderInfo);

        writeSocket(clientSock, "[CONFIRMATION]");
    } else writeSocket(clientSock, "[INVALID]");
}

void buyerViewsOrder(char *buffer, int* clientSock) {
    //Required Buffer String: "[VIEW_BUYER_ORDERS],int buyerID,"
    struct csvBillingInfo billingInfo[maxRowsInDB];
    char dataToSendClient[MSG_BUFFER_SIZE] = {0};
    int rowCount = 0;
    int userID;
    char numsToString[20] = {0};

    userID = atoi(strtok(buffer, ","));
    loadBillingInfo(billingInfo);

    while(billingInfo[rowCount].orderId != 0) {
        if(billingInfo[rowCount].customerId == userID) {
            sprintf(numsToString,"%d", billingInfo[rowCount].orderId);
            strcat(dataToSendClient, numsToString);
            strcat(dataToSendClient, ",");
            bzero(numsToString, 20);

            sprintf(numsToString,"%d", billingInfo[rowCount].totalOrderCost);
            strcat(dataToSendClient, numsToString);
            strcat(dataToSendClient, ",\n");
            bzero(numsToString, 20);
        }
        rowCount++;
    }
    writeSocket(clientSock,dataToSendClient);
}

void buyerViewsInfo(char *buffer, int* clientSock) {
    //Required Buffer String: "[VIEW_BILLING],int buyerID,"
    struct csvClientInfo clientInfo[maxRowsInDB];
    char dataToSendClient[MSG_BUFFER_SIZE] = {0};
    int rowCount = 0;
    int userID;
    char numsToString[20] = {0};

    userID = atoi(strtok(buffer, ","));
    loadClientInfo("CustomerInfo.txt", clientInfo);

    while(clientInfo[rowCount].uuid != 0) {
        if(clientInfo[rowCount].uuid == userID) {
            sprintf(numsToString,"%d", clientInfo[rowCount].uuid);
            strcat(dataToSendClient, numsToString);
            strcat(dataToSendClient, ",");
            bzero(numsToString, 20);

            strcat(dataToSendClient, clientInfo[rowCount].firstName);
            strcat(dataToSendClient, ",");

            strcat(dataToSendClient, clientInfo[rowCount].lastName);
            strcat(dataToSendClient, ",");

            strcat(dataToSendClient, clientInfo[rowCount].phoneNumber);
            strcat(dataToSendClient, ",");

            strcat(dataToSendClient, clientInfo[rowCount].streetAddress);
            strcat(dataToSendClient, ",");

            strcat(dataToSendClient, clientInfo[rowCount].city);
            strcat(dataToSendClient, ",");

            strcat(dataToSendClient, clientInfo[rowCount].state);
            strcat(dataToSendClient, ",");

            strcat(dataToSendClient, clientInfo[rowCount].zipCode);
            strcat(dataToSendClient, ",\n");
            bzero(numsToString, 20);
        }
        rowCount++;
    }
    writeSocket(clientSock,dataToSendClient);
}

void addNewProduct(char *buffer, int* clientSock) {
    //REQ Buffer string: "[NEW_PRODUCT],sellerID,productName,quantityAvailable,pricePerUnit,"
    struct csvProductInfo pList[maxRowsInDB];
    int newRecordRow = 0;
    char *ptr;
    loadProductInfo(pList);

    while( pList[newRecordRow].productId != 0){
        newRecordRow++;
    }

    pList[newRecordRow].productId = generateUID();

    ptr = strtok(buffer,",");
    pList[newRecordRow].sellerId = atoi(ptr);

    ptr = strtok(NULL, ",");
    strcpy(pList[newRecordRow].productName, ptr);

    ptr = strtok(NULL, ",");
    pList[newRecordRow].quantity = atoi(ptr);

    ptr = strtok(NULL, ",");
    pList[newRecordRow].price = atoi(ptr);

    //write plist to text file
    writeBackProductStruct(pList);

    writeSocket(clientSock, "[CONFIRMATION]");
}

void deleteProduct(char *buffer, int* clientSock) {
    //REQUIRED Buff string: "sellerID,productID,"

    struct csvProductInfo pList[maxRowsInDB];
    int rowCount = 0;
    int rowCountTrailing = 0;
    char *ptr;
    int sellerID, productID;
    char productIdToValidate[20] = {0};
    int productRowNum = 0;
    int delProductIndex = 0;

    ptr = strtok(buffer,",");
    sellerID = atoi(ptr);

    ptr = strtok(NULL, ",");
    strcpy(productIdToValidate, ptr);
    strcat(productIdToValidate, ",[PRODUCT],");
    productID = atoi(ptr);



    if(validateID(productIdToValidate)) {
        struct csvProductInfo modifiedProductList[maxRowsInDB];

        loadProductInfo(pList);

        //find row number of the product to delete
        while (pList[productRowNum].productId != 0) {
            if (pList[productRowNum].productId == productID) {
                break;
            }
            productRowNum++;
        }

        //check to make sure seller is owner of the product to delete
        if(pList[productRowNum].sellerId != sellerID) {
            writeSocket(clientSock, "[INVALID]");
        } else {
            //add to deleted product list
            while(deletedProducts[delProductIndex] != 0){
                delProductIndex++;
            }
            deletedProducts[delProductIndex] = productID;
            //load pList into a new table, but skip productRowNum
            while(pList[rowCount].productId != 0) {
                if(pList[rowCount].productId != productID){
                    modifiedProductList[rowCountTrailing] = pList[rowCount];
                } else {
                    rowCountTrailing -= 1;
                }
                rowCount++;
                rowCountTrailing++;
            }
        }
        //write modifiedProductList to ProductInfo.txt
        writeBackProductStruct(modifiedProductList);
        writeSocket(clientSock, "[CONFIRMATION]");

    } else {
        writeSocket(clientSock, "[INVALID]");
    }
}

void modifyQuantity(char *buffer, int* clientSock) {
    //REQ Buffer string: "sellerID,productID,quantityToSet"

    struct csvProductInfo pList[maxRowsInDB];
    char *ptr;
    int sellerID, productID, quantity;
    char productIdToValidate[20] = {0};
    int productRowNum = 0;

    ptr = strtok(buffer,",");
    sellerID = atoi(ptr);

    ptr = strtok(NULL, ",");
    strcpy(productIdToValidate, ptr);
    strcat(productIdToValidate, ",[PRODUCT],");
    productID = atoi(ptr);
    ptr = strtok(NULL, ",");
    quantity = atoi(ptr);


    if(validateID(productIdToValidate)) {

        loadProductInfo(pList);

        //find row number of the product to modify
        while (pList[productRowNum].productId != 0) {
            if (pList[productRowNum].productId == productID) {
                break;
            }
            productRowNum++;
        }

        //check to make sure seller is owner of the product to delete
        if(pList[productRowNum].sellerId != sellerID) {
            writeSocket(clientSock, "[INVALID]");
        } else {
            //modify the quantity
            pList[productRowNum].quantity = quantity;
        }
        //write plist to ProductInfo.txt
        writeBackProductStruct(pList);
        writeSocket(clientSock, "[CONFIRMATION]");

    } else {
        writeSocket(clientSock, "[INVALID]");
    }
}

void modifyPrice(char *buffer, int* clientSock) {
    //REQ Buffer string: "sellerID,productID,quantityToSet"

    struct csvProductInfo pList[maxRowsInDB];
    char *ptr;
    int sellerID, productID, priceToSet;
    char productIdToValidate[20] = {0};
    int productRowNum = 0;

    ptr = strtok(buffer,",");
    sellerID = atoi(ptr);

    ptr = strtok(NULL, ",");
    strcpy(productIdToValidate, ptr);
    strcat(productIdToValidate, ",[PRODUCT],");
    productID = atoi(ptr);
    ptr = strtok(NULL, ",");
    priceToSet = atoi(ptr);


    if(validateID(productIdToValidate)) {

        loadProductInfo(pList);

        //find row number of the product to modify
        while (pList[productRowNum].productId != 0) {
            if (pList[productRowNum].productId == productID) {
                break;
            }
            productRowNum++;
        }

        //check to make sure seller is owner of the product to delete
        if(pList[productRowNum].sellerId != sellerID) {
            writeSocket(clientSock, "[INVALID]");
        } else {
            //modify the quantity
            pList[productRowNum].price = priceToSet;
        }
        //write plist to ProductInfo.txt
        writeBackProductStruct(pList);
        writeSocket(clientSock, "[CONFIRMATION]");

    } else {
        writeSocket(clientSock, "[INVALID]");
    }
}

void sellerViewsOrders(char *buffer, int* clientSock) {
    //REQ Buffer string: "[SELLER_ORDERS],sellerID,"

    struct csvProductInfo pList[maxRowsInDB];
    struct csvCustomerOrderInfo customerOrderInfo[maxRowsInDB];
    char *ptr;
    int ownedProductIDs[maxRowsInDB] = {0};
    int sellerID;
    int rowCounter = 0;
    int arrayCounter = 0;
    char dataToSendClient[MSG_BUFFER_SIZE] = {0};
    char numsToString[20];

    ptr = strtok(buffer,",");
    sellerID = atoi(ptr);

    //load and extract all productIDs owned by the seller
    loadProductInfo(pList);

    while (pList[rowCounter].productId != 0) {
        if (pList[rowCounter].sellerId == sellerID) {
            ownedProductIDs[arrayCounter] = pList[rowCounter].productId;
            arrayCounter++;
        }
        rowCounter++;
    }
    //scan CustomerOrder database and add orders owned by seller to dataToSend
    loadCustomerOrderInfo(customerOrderInfo);
    for(int i = 0; i < arrayCounter; i++) {
        rowCounter = 0;
        while(customerOrderInfo[rowCounter].orderId != 0) {
            if (customerOrderInfo[rowCounter].productId == ownedProductIDs[i]) {
                sprintf(numsToString,"%d", customerOrderInfo[rowCounter].orderId);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");
                bzero(numsToString, 20);

                sprintf(numsToString,"%d", customerOrderInfo[rowCounter].productId);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");
                bzero(numsToString, 20);

                sprintf(numsToString,"%d", customerOrderInfo[rowCounter].quantityPurchased);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");
                bzero(numsToString, 20);

                sprintf(numsToString,"%d", customerOrderInfo[rowCounter].totalPrice);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");
                bzero(numsToString, 20);

                strcat(dataToSendClient, customerOrderInfo[rowCounter].firstName);
                strcat(dataToSendClient, ",");
                strcat(dataToSendClient, customerOrderInfo[rowCounter].lastName);
                strcat(dataToSendClient, ",");
                strcat(dataToSendClient, customerOrderInfo[rowCounter].streetAddress);
                strcat(dataToSendClient, ",");
                strcat(dataToSendClient, customerOrderInfo[rowCounter].city);
                strcat(dataToSendClient, ",");
                strcat(dataToSendClient, customerOrderInfo[rowCounter].state);
                strcat(dataToSendClient, ",");
                strcat(dataToSendClient, customerOrderInfo[rowCounter].zipCode);
                strcat(dataToSendClient, ",");
            }
            rowCounter++;
        }
    }
    writeSocket(clientSock, dataToSendClient);
}

void clientEditsInfo(char *buffer, int* clientSock) {
    /*REQ Buffer Strings:
    "[BUYER],clientID#,[NAME],firstName,lastName,"
    "[BUYER],clientID#,[NUMBER],phoneNum,"
    "[BUYER],clientID#,[ADDRESS],streetNum,city,state,zipCode,"
    "[SELLER],clientID#,[NAME],firstName,lastName,"
    "[SELLER],clientID#,[NUMBER],phoneNum,"
    "[SELLER],clientID#,[ADDRESS],streetNum,city,state,zipCode,"
    */

    char* ptr;
    char clientType[20] = {0};
    int clientID;
    char subCommand[20] = {0};

    ptr = strtok(buffer, ",");
    strcpy(clientType, ptr);

    ptr = strtok(NULL, ",");
    clientID = atoi(ptr);

    ptr = strtok(NULL, ",");
    strcpy(subCommand, ptr);

    ptr = strtok(NULL, "");

    if(strstr(clientType, "[BUYER]") != NULL) {
        if(strstr(subCommand, "[NAME]") != NULL) {
            buyerEditsName(ptr, clientID);
        } else if(strstr(subCommand, "[NUMBER]") != NULL) {
            buyerEditsNumber(ptr, clientID);
        } else if(strstr(subCommand, "[ADDRESS]") != NULL) {
            buyerEditsAddress(ptr, clientID);
        }
    } else {
        if(strstr(subCommand, "[NAME]") != NULL) {
            sellerEditsName(ptr, clientID);
        } else if(strstr(subCommand, "[NUMBER]") != NULL) {
            sellerEditsNumber(ptr, clientID);
        } else if(strstr(subCommand, "[ADDRESS]") != NULL) {
            sellerEditsAddress(ptr, clientID);
        }
    }
    writeSocket(clientSock, "[CONFIRMATION]");
}

void sellerEditsName(char* buffer, int id) {
    char* ptr;
    struct csvClientInfo sellerList[maxRowsInDB];
    int sellerRowInDB = 0;

    loadClientInfo("SellerInfo.txt", sellerList);
    while(sellerList[sellerRowInDB].uuid != id) {
        sellerRowInDB++;
    }

    ptr = strtok(buffer, ",");
    bzero(sellerList[sellerRowInDB].firstName, 20);
    strcpy(sellerList[sellerRowInDB].firstName, ptr);

    ptr = strtok(NULL, ",");
    bzero(sellerList[sellerRowInDB].lastName, 20);
    strcpy(sellerList[sellerRowInDB].lastName, ptr);

    //writeback sellerList to SellerInfo.txt
    writeBackClientStruct("SellerInfo.txt", sellerList);
}

void sellerEditsNumber(char* buffer, int id) {
    char* ptr;
    struct csvClientInfo sellerList[maxRowsInDB];
    int sellerRowInDB = 0;

    loadClientInfo("SellerInfo.txt", sellerList);
    while(sellerList[sellerRowInDB].uuid != id) {
        sellerRowInDB++;
    }

    ptr = strtok(buffer, ",");
    bzero(sellerList[sellerRowInDB].phoneNumber,20);
    strcpy(sellerList[sellerRowInDB].phoneNumber, ptr);

    //writeback sellerList to SellerInfo.txt
    writeBackClientStruct("SellerInfo.txt", sellerList);
}

void sellerEditsAddress(char* buffer, int id) {
    char* ptr;
    struct csvClientInfo sellerList[maxRowsInDB];
    int sellerRowInDB = 0;

    loadClientInfo("SellerInfo.txt", sellerList);
    while(sellerList[sellerRowInDB].uuid != id) {
        sellerRowInDB++;
    }

    ptr = strtok(buffer, ",");
    bzero(sellerList[sellerRowInDB].streetAddress,20);
    strcpy(sellerList[sellerRowInDB].streetAddress, ptr);

    ptr = strtok(NULL, ",");
    bzero(sellerList[sellerRowInDB].city,20);
    strcpy(sellerList[sellerRowInDB].city, ptr);

    ptr = strtok(NULL, ",");
    bzero(sellerList[sellerRowInDB].state,20);
    strcpy(sellerList[sellerRowInDB].state, ptr);

    ptr = strtok(NULL, ",");
    bzero(sellerList[sellerRowInDB].zipCode,20);
    strcpy(sellerList[sellerRowInDB].zipCode, ptr);

    //writeback sellerList to SellerInfo.txt
    writeBackClientStruct("SellerInfo.txt", sellerList);
}

void buyerEditsNumber(char* buffer, int id) {
    char* ptr;
    struct csvClientInfo buyerList[maxRowsInDB];
    int buyerRowInDB = 0;

    loadClientInfo("CustomerInfo.txt", buyerList);
    while(buyerList[buyerRowInDB].uuid != id) {
        buyerRowInDB++;
    }

    ptr = strtok(buffer, ",");
    bzero(buyerList[buyerRowInDB].phoneNumber,20);
    strcpy(buyerList[buyerRowInDB].phoneNumber, ptr);

    //writeback buyerList to CustomerInfo.txt
    writeBackClientStruct("CustomerInfo.txt", buyerList);
}

void buyerEditsName(char* buffer, int id) {
    char* ptr;
    struct csvClientInfo buyerList[maxRowsInDB];
    struct csvBillingInfo billingInfo[maxRowsInDB];
    struct csvCustomerOrderInfo customerOrderInfo[maxRowsInDB];
    int buyerRowInDB = 0;
    int buyerOrderNums[maxRowsInDB];
    int orderCount = 0;
    int rowCount = 0;
    char firstName[20] = {0};
    char lastName[20] = {0};

    //edit name in CustomerInfo
    loadClientInfo("CustomerInfo.txt", buyerList);
    while(buyerList[buyerRowInDB].uuid != id) {
        buyerRowInDB++;
    }

    ptr = strtok(buffer, ",");
    strcpy(firstName, ptr);
    bzero(buyerList[buyerRowInDB].firstName, 20);
    strcpy(buyerList[buyerRowInDB].firstName, firstName);

    ptr = strtok(NULL, ",");
    strcpy(lastName, ptr);
    bzero(buyerList[buyerRowInDB].lastName, 20);
    strcpy(buyerList[buyerRowInDB].lastName, lastName);

    //write back buyerList to CustomerInfo.txt
    writeBackClientStruct("CustomerInfo.txt", buyerList);

    //edit name in BillingInfo.txt
    loadBillingInfo(billingInfo);
    while(billingInfo[rowCount].orderId != 0) {
        if(billingInfo[rowCount].customerId == id) {

            buyerOrderNums[orderCount] = billingInfo[rowCount].orderId;
            orderCount++;

            bzero(billingInfo[rowCount].firstName, 20);
            strcpy(billingInfo[rowCount].firstName, firstName);

            bzero(billingInfo[rowCount].lastName, 20);
            strcpy(billingInfo[rowCount].lastName, lastName);
        }
        rowCount++;
    }
    //write back billingInfo to BillingInfo.txt
    writeBackBillingStruct(billingInfo);

    //edit name in CustomerOrder.txt
    loadCustomerOrderInfo(customerOrderInfo);
    for(int i = 0; i < orderCount; i++) {
        rowCount = 0;
        while(customerOrderInfo[rowCount].orderId != 0) {
            if(customerOrderInfo[rowCount].orderId == buyerOrderNums[i]) {
                bzero(customerOrderInfo[rowCount].firstName, 20);
                strcpy(customerOrderInfo[rowCount].firstName, firstName);

                bzero(customerOrderInfo[rowCount].lastName, 20);
                strcpy(customerOrderInfo[rowCount].lastName, lastName);
            }
            rowCount++;
        }
    }
    //write back customerOrderInfo to CustomerOrderInfo.txt
    writeBackCustomerOrderStruct(customerOrderInfo);
}

void buyerEditsAddress(char* buffer, int id) {
    char* ptr;
    struct csvClientInfo buyerList[maxRowsInDB];
    struct csvBillingInfo billingInfo[maxRowsInDB];
    struct csvCustomerOrderInfo customerOrderInfo[maxRowsInDB];
    int buyerRowInDB = 0;
    int buyerOrderNums[maxRowsInDB];
    int orderCount = 0;
    int rowCount = 0;
    char streetNum[20] = {0};
    char city[20] = {0};
    char state[20] = {0};
    char zipCode[20] = {0};

    //edit name in CustomerInfo
    loadClientInfo("CustomerInfo.txt", buyerList);
    while(buyerList[buyerRowInDB].uuid != id) {
        buyerRowInDB++;
    }

    ptr = strtok(buffer, ",");
    strcpy(streetNum, ptr);
    bzero(buyerList[buyerRowInDB].streetAddress, 20);
    strcpy(buyerList[buyerRowInDB].streetAddress, streetNum);

    ptr = strtok(NULL, ",");
    strcpy(city, ptr);
    bzero(buyerList[buyerRowInDB].city, 20);
    strcpy(buyerList[buyerRowInDB].city, city);

    ptr = strtok(NULL, ",");
    strcpy(state, ptr);
    bzero(buyerList[buyerRowInDB].state, 20);
    strcpy(buyerList[buyerRowInDB].state, state);

    ptr = strtok(NULL, ",");
    strcpy(zipCode, ptr);
    bzero(buyerList[buyerRowInDB].zipCode, 20);
    strcpy(buyerList[buyerRowInDB].zipCode, zipCode);

    //write back buyerList to CustomerInfo.txt
    writeBackClientStruct("CustomerInfo.txt", buyerList);

    //edit name in BillingInfo.txt
    loadBillingInfo(billingInfo);
    while(billingInfo[rowCount].orderId != 0) {
        if(billingInfo[rowCount].customerId == id) {

            buyerOrderNums[orderCount] = billingInfo[rowCount].orderId;
            orderCount++;

            bzero(billingInfo[rowCount].streetAddress, 20);
            strcpy(billingInfo[rowCount].streetAddress, streetNum);

            bzero(billingInfo[rowCount].city, 20);
            strcpy(billingInfo[rowCount].city, city);

            bzero(billingInfo[rowCount].state, 20);
            strcpy(billingInfo[rowCount].state, state);

            bzero(billingInfo[rowCount].zipCode, 20);
            strcpy(billingInfo[rowCount].zipCode, zipCode);
        }
        rowCount++;
    }
    //write back billingInfo to BillingInfo.txt
    writeBackBillingStruct(billingInfo);

    //edit name in CustomerOrder.txt
    loadCustomerOrderInfo(customerOrderInfo);
    for(int i = 0; i < orderCount; i++) {
        rowCount = 0;
        while(customerOrderInfo[rowCount].orderId != 0) {
            if(customerOrderInfo[rowCount].orderId == buyerOrderNums[i]) {
                bzero(customerOrderInfo[rowCount].streetAddress, 20);
                strcpy(customerOrderInfo[rowCount].streetAddress, streetNum);

                bzero(customerOrderInfo[rowCount].city, 20);
                strcpy(customerOrderInfo[rowCount].city, city);

                bzero(customerOrderInfo[rowCount].state, 20);
                strcpy(customerOrderInfo[rowCount].state, state);

                bzero(customerOrderInfo[rowCount].zipCode, 20);
                strcpy(customerOrderInfo[rowCount].zipCode, zipCode);
            }
            rowCount++;
        }
    }
    //write back customerOrderInfo to CustomerOrderInfo.txt
    writeBackCustomerOrderStruct(customerOrderInfo);
}

void buyerModifiesOrder(char *buffer, int* clientSock) { //TODO STILL NEED TO TEST
    //Buff String: "orderID,productIDToReturn,"
    struct csvCustomerOrderInfo customerOrderInfo[maxRowsInDB];
    struct csvBillingInfo billingInfo[maxRowsInDB];
    struct csvCustomerOrderInfo modifiedCustomerOrderInfo[maxRowsInDB];
    char* ptr;
    int productIDtoReturn;
    int orderID;
    int itemCost;
    int rowCount = 0;
    int rowCountTrailing = 0;
    int productRowLoc = 0;

    ptr = strtok(buffer, ",");
    orderID = atoi(ptr);

    ptr = strtok(NULL, ",");
    productIDtoReturn = atoi(ptr);

    loadCustomerOrderInfo(customerOrderInfo);
    loadBillingInfo(billingInfo);

    //find the product to remove in CustomerOrderInfo
    while(customerOrderInfo[productRowLoc].orderId != orderID) {
        productRowLoc++;
    }
    //get item cost to deduct from the BillingInfo DB
    itemCost = customerOrderInfo[productRowLoc].totalPrice;

    //modify customerOrderInfoDB
    //load pList into a new table, but skip productRowNum
    while(customerOrderInfo[rowCount].productId != 0) {
        if(customerOrderInfo[rowCount].productId != productIDtoReturn){
            modifiedCustomerOrderInfo[rowCountTrailing] = customerOrderInfo[rowCount];
        } else {
            rowCountTrailing -= 1;
        }
        rowCount++;
        rowCountTrailing++;
    }

    //write modifiedCustomerOrderInfo to CustomerOrderInfo.txt
    writeBackCustomerOrderStruct(modifiedCustomerOrderInfo);

    //find the order in BillingInfo and deduct deleted product price from the total order price
    rowCount = 0;
    while(billingInfo[rowCount].orderId != orderID) {
        rowCount++;
    }
    billingInfo[rowCount].totalOrderCost -= itemCost;

    //write billingInfo to BillingInfo.txt
    writeBackBillingStruct(billingInfo);
}


void readOrderDetails(char *buffer, int* clientSock) {
    struct itemOrder itemsInOrder[200];
    struct csvCustomerOrderInfo customerOrderInfo[maxRowsInDB];
    struct csvProductInfo pList[maxRowsInDB];
    int rowCount = 0;
    char* ptr;
    int orderID;
    int numOfProducts = 0;
    char dataToSendClient[MSG_BUFFER_SIZE] = {0};
    char numsToString[20];

    ptr = strtok(buffer, ",");
    orderID = atoi(ptr);

    //get productIds, quantity purchased, and item price total
    loadCustomerOrderInfo(customerOrderInfo);
    while(customerOrderInfo[rowCount].orderId != 0) {
        if (customerOrderInfo[rowCount].orderId == orderID) {
           itemsInOrder[numOfProducts].productID = customerOrderInfo[rowCount].productId;
           itemsInOrder[numOfProducts].quantity = customerOrderInfo[rowCount].quantityPurchased;
           itemsInOrder[numOfProducts].itemCostPerUnit = customerOrderInfo[rowCount].totalPrice;
           numOfProducts++;
        }
        rowCount++;
    }
    //getting product names
    loadProductInfo(pList);
    for(int i = 0; i < numOfProducts; i++) {
        rowCount = 0;
        while(pList[rowCount].productId != 0) {
            if(itemsInOrder[i].productID == pList[rowCount].productId) {

                sprintf(numsToString,"%d", itemsInOrder[i].productID);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");
                bzero(numsToString, 20);

                strcat(dataToSendClient, pList[rowCount].productName);
                strcat(dataToSendClient, ",");

                sprintf(numsToString,"%d", itemsInOrder[i].quantity);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");
                bzero(numsToString, 20);

                sprintf(numsToString,"%d", itemsInOrder[i].itemCostPerUnit);
                strcat(dataToSendClient, numsToString);
                strcat(dataToSendClient, ",");
                bzero(numsToString, 20);

            }
            rowCount++;
        }
    }
    writeSocket(clientSock, dataToSendClient);
}

bool validateOrderIsBuyers(char *buffer, int* clientSock) {  //TODO NEED TO TEST
    //Buff String: buyerID,orderID,
    int buyerID, orderID;
    char* ptr;
    struct csvBillingInfo billingInfo[maxRowsInDB];
    int rowCounter = 0;

    ptr = strtok(buffer, ",");
    buyerID = atoi(ptr);

    ptr = strtok(NULL, ",");
    orderID = atoi(ptr);

    loadBillingInfo(billingInfo);

    while(billingInfo[rowCounter].orderId != 0) {
        if(billingInfo[rowCounter].orderId == orderID) {
            if(billingInfo[rowCounter].customerId == buyerID) {
                return true;
            } else
                return false;
        }
    }
    return false;
}

void executeTaskOnServer(int clientSock) {
    char command[25];
    char buffer[MSG_BUFFER_SIZE];

    bzero(buffer, MSG_BUFFER_SIZE);
    readSocket(&clientSock, buffer);
    extractCommand(buffer, command);

    if(strstr(command, "[NEW_CLIENT]") != NULL) {
        /*1.store new client data  TCP COMMAND: [NEW_CLIENT]*/
        //REQUIRED BUFFER STRING: "[NEW_CLIENT],[BUYER],FirstName,LastName,phoneNumber,streetAddress,City,State,zipcode,"  <--notice buffer should end with ','
        //OR REQUIRED BUFFER STRING: "[NEW_CLIENT],[SELLER],FirstName,LastName,phoneNumber,streetAddress,City,State,zipcode,"

        writeNewClientData(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: newClientID

    } else if(strstr(command, "[VALIDATE_ID]") != NULL) {
        /*2.Validate an ID in a database  TCP COMMAND: [VALIDATE_ID]*/
        /*Required Buffer string: "[VALIDATE_ID],buyerID,[BUYER],"
                                or "[VALIDATE_ID],sellerID,[SELLER],"
                                or "[VALIDATE_ID],productID,[PRODUCT],"
                                or "[VALIDATE_ID],orderID,[BILLING],"
                                or "[VALIDATE_ID],productID,[ORDER],"          */
        bool result;
        result = validateID(buffer);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        if(result) {
            writeSocket(&clientSock, "[CONFIRMATION]");
        } else
            writeSocket(&clientSock, "[INVALID]");

        //SENDS BACK: [CONFIRMATION] OR [INVALID]

    } else if(strstr(command, "[VIEW_PRODUCTS]") != NULL) {
        /*3. View Available Products TCP COMMAND: [VIEW_PRODUCTS]*/
        //REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[BUYER],"
        //OR REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[SELLER],sellerID,"

        viewProducts(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SEND ALL: ProductIDs, Product Names, Quantitys, Price/Units*/

    } else if(strstr(command, "[COMPLETE_ORDER]") != NULL) {
        /*4. BuyerOPTION 1-3 Complete Order TCP COMMAND: [COMPLETE_ORDER]*/
        //Required Buffer String: "[COMPLETE_ORDER],int buyerID,int productID,int quantityOrdered,int pricePerUnit,int productID,int quantityOrdered,int pricePerUnit, ...,"

        completeAnOrder(buffer,&clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: [CONFIRMATION] OR [INVALID]

    } else if(strstr(command, "[VIEW_BUYER_ORDERS]") != NULL) {
        /*5. BuyerOPTION 2 View Orders TCP COMMAND: [VIEW_BUYER_ORDERS]*/
        //Required Buffer String: "[VIEW_BUYER_ORDERS],int buyerID,"

        buyerViewsOrder(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: orderID,TotalOrderCost,

    } else if(strstr(command, "[CHECK_BUYER]") != NULL) {
        /*6. BuyerOPTION 3 Modify Order TCP COMMAND: [MOD_ORDER]*/
        //REQ Buffer String: [CHECK_BUYER],buyerID,orderID,

        bool result = validateOrderIsBuyers(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        if(result)
            writeSocket(&clientSock, "[CONFIRMATION]");
        else
            writeSocket(&clientSock, "[INVALID]");

        //SENDS BACK: [INVALID] OR [CONFIRMATION]


    }else if(strstr(command, "[ORDER_DETAILS]") != NULL) {
        /*6. BuyerOPTION 3 Modify Order TCP COMMAND: [ORDER_DETAILS]*/
        //!!clientServer calls [CHECK_BUYER] first!!"
        //REQ Buffer String: "[ORDER_DETAILS],numberID,"

        readOrderDetails(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: productID,Product Name,quantityOrdered,TotalPrice,

    } else if(strstr(command, "[MOD_ORDER]") != NULL) {
        /*6. BuyerOPTION 3 Modify Order TCP COMMAND: [MOD_ORDER]*/
        //1.clientServer validates order belongs to buyer: [CHECK_BUYER],buyerID,orderID],
        //2.clientServer views order details: "[ORDER_DETAILS],orderID,"
        //3.clientServer validates productID:  "[VALIDATE_ID],productIDToReturn,[ORDER],"
        //4.lastly, clientServer calls "[MOD_ORDER],orderID,productIDToReturn"

        buyerModifiesOrder(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: CONFIRMATION]


    } else if(strstr(command, "[VIEW_BILLING]") != NULL) {
        /*7. BuyerOPTION 4 View Billing Info TCP COMMAND: [VIEW_BILLING]*/
        //Required Buffer String: "[VIEW_BILLING],int buyerID,"

        buyerViewsInfo(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: buyerID,firstName,LastName,PhoneNumber,StreetAddress,city,state,zipCode

    } else if(strstr(command, "[EDIT_INFO]") != NULL) {
        /*8. BuyerOPTION/SellerOPTION 5 Edit Info TCP COMMAND: [EDIT_INFO]*/
        /*REQ Buffer Strings:
         "[EDIT_INFO],[BUYER],buyerID#,[NAME],firstName,lastName,"
         "[EDIT_INFO],[BUYER],buyerID#,[NUMBER],phoneNum,"
         "[EDIT_INFO],[BUYER],buyerID#,[ADDRESS],streetNum,city,state,zipCode,"
         "[EDIT_INFO],[SELLER],sellerID#,[NAME],firstName,lastName,"
         "[EDIT_INFO],[SELLER],sellerID#,[NUMBER],phoneNum,"
         "[EDIT_INFO],[SELLER],sellerID#,[ADDRESS],streetNum,city,state,zipCode,"
         */

        clientEditsInfo(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: [CONFIRMATION]

    } else if(strstr(command, "[NEW_PRODUCT]") != NULL) {
        /*9. SellerOPTION 2 Add New Product TCP COMMAND: [NEW_PRODUCT]*/
        //REQ Buffer string: "[NEW_PRODUCT],sellerID,productName,quantityAvailable,pricePerUnit,"

        addNewProduct(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: [CONFIRMATION]

    } else if(strstr(command, "[DELETE_PROD]") != NULL) {
        /*10. SellerOption 3 Delete Product TCP COMMAND: [DELETE_PROD]*/
        //REQUIRED Buff string: "[DELETE_PROD],sellerID,productID,"

        deleteProduct(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: [INVALID] OR [CONFIRMATION]

    } else if(strstr(command, "[MOD_QUANT]") != NULL) {
        /*11. SellerOption 4 Modify Product Quantity TCP COMMAND: [MOD_QUANT]*/
        //REQ Buffer string: "[MOD_QUANT],sellerID,productID,quantityToSet"

        modifyQuantity(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: [INVALID] OR [CONFIRMATION]

    } else if(strstr(command, "[MOD_PRICE]") != NULL) {
        /*12. SellerOption 5 Modify Product Price TCP COMMAND: [MOD_PRICE]*/
        //REQ Buffer string: "[MOD_PRICE],sellerID,productID,priceToSet"

        modifyPrice(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: [INVALID] OR [CONFIRMATION]

    } else if(strstr(command, "[SELLER_ORDERS]") != NULL) {
        /*13. SellerOption 6 View Product Orders TCP COMMAND: [SELLER_ORDERS]*/
        //REQ Buffer string: "[SELLER_ORDERS],sellerID,"

        sellerViewsOrders(buffer, &clientSock);
        bzero(buffer, MSG_BUFFER_SIZE);
        bzero(command, 25);

        //SENDS BACK: orderID,productID,quantityPurchased,totalPrice,firstName,lastName,streetAddress,city,state,zipCode,
    }
}

#pragma clang diagnostic pop