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

    if(read(*sock, buffer, MSG_BUFFER_SIZE) == -1) {
        printf("failed to read socket\n");
        exit(1);
    }
}

void writeSocket(const int* sock, char* buffer) {

    if (write(*sock, buffer, MSG_BUFFER_SIZE) == -1) {
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
    int idNum = (rand() % 9000) + 1000;

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
    /*Required Buffer string: "idNumber,[BUYER]"
                        or "idNumber,[SELLER]"
                        or "idNumber,[PRODUCT]"
                        or "idNumber,[BILLING]"          */
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
                if (buyerList[rowCount].uuid == idToValidate) {return true; };
                rowCount++;
            }
        }
        return false; // returns false if a problem or if ID is not present

    } else if (strstr(dbName, "[SELLER]") != NULL) {
        struct csvClientInfo sellerList[maxRowsInDB];
        if (loadClientInfo("CustomerInfo.txt", sellerList)) {
            while (sellerList[rowCount].uuid != 0) {
                if (sellerList[rowCount].uuid == idToValidate) { return true; };
                rowCount++;
            }
        }
        return false;

    } else if (strstr(dbName, "[PRODUCT]") != NULL) {
        struct csvProductInfo productList[maxRowsInDB];
        if (loadProductInfo(productList)) {
            while (productList[rowCount].productId != 0) {
                if (productList[rowCount].productId == idToValidate) { return true; };
                rowCount++;
            }
        }
        return false;

    } else if (strstr(dbName, "[BILLING]") != NULL) {
        struct csvBillingInfo billingList[maxRowsInDB];
        if (loadBillingInfo(billingList)) {
            while (billingList[rowCount].orderId != 0) {
                if (billingList[rowCount].orderId == idToValidate) { return true; };
                rowCount++;
            }
        }
        return false;

    } else return false;
    // returns false if a problem or if ID is not present
    /*2. bool validateID(int ID, string DatabaseName) TCP COMMAND: [VALIDATE_ID]
    if ID in DatabaseName
    return true
    else
    return false*/
}

void writeNewClientData(char *buffer) {
    int uuid;
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


    /*IF new Buyer
    WRITE CustomerInformation.txt
    NEW CustomerID, FirstName, LastName, phoneNumber, streetAddress, City, State, zipcode
    SEND Confirmation*/
    /*
    IF new Seller
    WRITE SellerInformation.txt
    NEW SellerID, FirstName, LastName, phoneNumber, streetAddress, City, State, zipcode
    SEND Confirmation*/
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

        //TODO: now write clientDB array back to its appropriate source.
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

        //TODO: now write clientDB array back to its appropriate source.
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

    //TODO write pList to ProductInfo.txt
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

    //write table to text file
    //TODO pass billingList to function that writes it to BillingInfo.txt

    return billingList[rowToWriteRecord].orderId;
}

void writeOrderToCustomerOrder(int orderID, struct csvClientInfo buyerData, struct itemOrder orderList[], struct csvCustomerOrderInfo customerOrders[]) {
    int lastRecordRow = 0;
    int totalOrderCost = 0;
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

    //TODO: write customerOrders to text file

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
        int buyerRecordRow = 0;
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

    //TODO write plist to text file

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
        //TODO write modifiedProductList to ProductInfo.txt
        writeSocket(clientSock, "[CONFIRMATION]");

    } else {
        writeSocket(clientSock, "[INVALID]");
    }
}

void modifyQuantity(char *buffer, int* clientSock) {
    //REQ Buffer string: "sellerID,productID,quantityToSet"

    struct csvProductInfo pList[maxRowsInDB];
    int rowCount = 0;
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
        //TODO write plist to ProductInfo.txt
        writeSocket(clientSock, "[CONFIRMATION]");

    } else {
        writeSocket(clientSock, "[INVALID]");
    }
}

void modifyPrice(char *buffer, int* clientSock) {
    //REQ Buffer string: "sellerID,productID,quantityToSet"

    struct csvProductInfo pList[maxRowsInDB];
    int rowCount = 0;
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
        //TODO write plist to ProductInfo.txt
        writeSocket(clientSock, "[CONFIRMATION]");

    } else {
        writeSocket(clientSock, "[INVALID]");
    }
}

