
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
#include "readDatabaseIntoArray.h"
#include "tcpStuff.h"
#include "dataServer.h"

/*---Server Command Declarations---*/



int main() {
    /* TCP Vars */
    int hSocket;
    int clientSock;
    struct sockaddr_in server = {0};
    struct sockaddr_in client = {0};
    char buffer[MSG_BUFFER_SIZE];
    char command[25];
    pid_t pid;


    createSocket(&hSocket);                                             //Creating socket for server to communicate through
    bindSocket(&server, &hSocket);                                      // binding socket to address
    listenOnSocket(&hSocket, CLIENT_BACKLOG);                           //Listen for clients, maximum of 3 connections


    /*----Forking Clients----  TODO: No longer forking, will use thread pool in future*/
    while (1) {
        if (acceptConnection(&clientSock, &hSocket, &client) == true) { //Accept incoming client connection
            pid = fork();                                               //Fork client process
            if(pid == 0) {                                              //code for child(client) process
                close(hSocket);


                /*---Main Logic Loop: Driven by Client Commands ---*/
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
                while(1) {
                    bzero(buffer, MSG_BUFFER_SIZE);
                    readSocket(&clientSock, buffer);
                    extractCommand(buffer, command);

                    /*1.store new client data  TCP COMMAND: [NEW_CLIENT]*/
                    //REQUIRED BUFFER STRING: "[NEW_CLIENT],[BUYER],FirstName,LastName,phoneNumber,streetAddress,City,State,zipcode,"  <--notice buffer should end with ','
                    //OR REQUIRED BUFFER STRING: "[NEW_CLIENT],[SELLER],FirstName,LastName,phoneNumber,streetAddress,City,State,zipcode,"
                    if(strstr(command, "[NEW_CLIENT]") != NULL) {

                        writeNewClientData(buffer);              //send modified buffer to extract contents and write to ConsumerInfo or SellerInfo.txt
                        writeSocket(&clientSock, "[CONFIRMATION]"); //Send confirmation to clientServer that info was added
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    /*2.Validate an ID in a database  TCP COMMAND: [VALIDATE_ID]*/
                    } else if(strstr(command, "[VALIDATE_ID]") != NULL) {

                        validateID(buffer);

                        /*


                    3. bool validateQuantity(int productID, int orderAmount)<---always checks ProductInfo.txt
                             if productID's quantity in ProductInfo.txt >= orderAmount
                                return true
                             else
                                return false

                    //Validation Scenarios:
                   *** CLIENT LOGIN: ***
                        IF Buyer -- data received from Client: CustomerID
                            READ CustomerInformation.txt
                            CustomerID -- present
                            //validateID(CustomerID, "CustomerInformation.txt") TCP COMMAND: [VALIDATE_ID]
                            SEND Confirmation
                        ELSE
                            SEND Invalid

                        IF Seller  -- data received from Client: SellerID
                             READ SellerInformation.txt
                             SellerID -- present
                             //validateID(SellerID, "SellerInformation.txt") TCP COMMAND: [VALIDATE_ID]
                             SEND Confirmation
                        ELSE
                            SEND Invalid

                    *** BUYER COMPLETING AN ORDER ***
                        Buyer submits a list of product ID's with desired quantity of each productID

                        for every Product ID
                            validateID(ProductID, "ProductInfo.txt") AND validateQuantity(int ProductID, int quantity) == false
                                return false

                    *** BUYER MODIFYING AN ORDER ***
                        validateID(OrderID, "BillingInfo.txt")
                        validateID(OrderID, ProductID, "CustomerOrder.txt") NEED TO OVERLOAD THIS FUNCTION FOR IT TO WORK with CustomerOrder.txt

                    *** Seller DELETES A PRODUCT ***
                        validateID(productID, ProductInfo.txt)

                    *** Seller MODIFIES A PRODUCT AMOUNT ***
                        validateID(productID, ProductInfo.txt)

                    *** Seller MODIFIES A PRODUCT PRICE ***
                        validateID(productID, ProductInfo.txt)
                         */

                    /*3. View Available Products TCP COMMAND: [VIEW_PRODUCTS]*/
                    //REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[BUYER],"
                    //OR REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[SELLER],uuid,"
                    } else if(strstr(command, "[VIEW_PRODUCTS]") != NULL) {

                        viewProducts(buffer, &clientSock);

                    /*4. BuyerOPTION 1-3 Complete Order TCP COMMAND: [COMPLETE_ORDER]*/
                    } else if(strstr(command, "[COMPLETE_ORDER]") != NULL) {


                        /*READ ProductInfo.txt validate given ProductID's AND Quantities
                        //validateID productID, "ProductInfo.txt")
                        //validateQuantity(int ProductID, int quantity)

                        IF OK --> WRITE CustomerOrder.txt AND WRITE BillingInfo.txt
                            SEND Order Conformation
                        ELSE Invalid
                            SEND Order Invalid*/


                    /*5. BuyerOPTION 2 View Orders TCP COMMAND: [VIEW_BUYER_ORDERS]*/

                    } else if(strstr(command, "[VIEW_BUYER_ORDERS]") != NULL) {


                        /*READ BillingInfo.txt
                        SEND OrderID, Total Order Price*/


                    /*6. BuyerOPTION 3 Modify Order TCP COMMAND: [MOD_ORDER]*/
                    } else if(strstr(command, "[MOD_ORDER]") != NULL) {


                        /*OrderID -- present in READ BillingInfo.txt //validateID(OrderID, "BillingInfo.txt")
                        IF OK --> READ CustomerOrder.txt AND READ BillingInfo.txt
                            SEND ProductID, Product Name, quantity, Total Price

                        ClientOPTION 3-1 Return a product
                        productID present in READ CustomerOrder.txt //validateID(OrderID, ProductID, "CustomerOrder.txt") NEED TO OVERLOAD THIS FUNCTION FOR IT TO WORK
                        IF OK --> WRITE CustomerOrder.txt AND WRITE BillingInfo.txt AND WRITE ProductInfo.txt
                            Send Order Modified
                        ELSE
                            SEND Invalid
                        ELSE Invalid
                            SEND Order Invalid*/


                    /*7. BuyerOPTION 4 View Billing Info TCP COMMAND: [VIEW_BILLING]*/
                    } else if(strstr(command, "[VIEW_BILLING]") != NULL) {


                       /* READ CustomerInfo.txt
                        SEND Name, Number, Address*/


                    /*8. BuyerOPTION/SellerOPTION 5 Edit Info TCP COMMAND: [EDIT_INFO]*/
                    } else if(strstr(command, "[EDIT_INFO]") != NULL) {


                        /* READ CustomerInfo.txt
                         IF Buyer
                            IF EditName TCP COMMAND: [NAME]
                                WRITE CustomerInfo.txt
                                SEND Confirmation
                            ELSE Number TCP COMMAND: [NUMBER]
                                WRITE CustomerInfo.txt
                                SEND Confirmation
                            ELSE Address TCP COMMAND: [ADDRESS]
                                WRITE CustomerInfo.txt
                                WRITE BillingInfo.txt
                                WRITE CustomerOrder.txt
                                SEND Confirmation
                        ELSE IF Seller
                        IF EditName TCP COMMAND: [NAME]
                            WRITE SellerInfo.txt
                            SEND Confirmation
                        ELSE Number TCP COMMAND: [NUMBER]
                            WRITE SellerInfo.txt
                            SEND Confirmation
                        ELSE Address TCP COMMAND: [ADDRESS]
                            WRITE SellerInfo.txt
                            SEND Confirmation*/


                    /*9. SellerOPTION 2 Add New Product TCP COMMAND: [NEW_PRODUCT]*/
                    } else if(strstr(command, "[NEW_PRODUCT]") != NULL) {

                       /* WRITE ProductInfo.txt
                        new ProductID, productName, SellerID, Quantity, Price/Unit
                        SEND Confirmation*/


                    /*10. SellerOption 3 Delete Product TCP COMMAND: [DELETE_PROD]*/
                    } else if(strstr(command, "[DELETE_PROD]") != NULL) {

                        /* READ ProductInfo.txt
                        validate ProductID //validateID(productID, ProductInfo.txt) TCP COMMAND:

                        IF OK --> WRITE ProductInfo.txt AND WRITE BillingInfo.txt AND WRITE CustomerOrder.txt
                            delete from ProductInfo
                            update CustomerOrder.txt and store total item price
                            update BillingInfo.txt total Order Price
                            SEND Confirmation
                        ELSE
                            SEND Invalid*/



                    /*11. SellerOption 4 Modify Product Quantity TCP COMMAND: [MOD_QUANT]*/
                    } else if(strstr(command, "[MOD_QUANT]") != NULL) {
                        printf("Seller modifying product quantity!\n"); //for testing
                        /* READ ProductInfo.txt
                        validate ProductID //validateID(productID, "ProductInfo.txt")

                        IF OK --> WRITE ProductInfo.txt
                            SEND Confirmation
                        ELSE
                            SEND InvalidID*/


                    /*12. SellerOption 5 Modify Product Price TCP COMMAND: [MOD_PRICE]*/
                    } else if(strstr(command, "[MOD_PRICE]") != NULL) {
                        printf("Seller modifying product price!\n"); //for testing
                        /* READ ProductInfo.txt
                        validate ProductID //validateID(productID, ProductInfo.txt)

                        IF OK --> WRITE ProductInfo.txt
                            adjust Product Price
                            SEND Confirmation
                        ELSE
                            SEND Invalid ID*/


                    /*13. SellerOption 6 View Product Orders TCP COMMAND: [SELLER_ORDERS]*/
                    } else if(strstr(command, "[SELLER_ORDERS]") != NULL) {
                        printf("Seller looking at their product orders!\n"); //for testing
                        /*  READ ProductInfo.txt
                        get ProductIDList for seller's ID

                        READ CustomerOrder.txt
                        SEND Filtered to ProductIDList: OrderID,ProductID, Quantity, Total Price, Delivery Address*/

                    }
                }
#pragma clang diagnostic pop

            } else if(pid > 0) {                                        //I'm the parent, Do parent things
                close(clientSock);                                      //close childSocket, parent uses hSocket
                wait(NULL);                                     //reap children
            } else {
                printf("Fork failed!\n");
                exit(1);
            }
        }
    }
    return 0;
}

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

    FILE *fstream = fopen("/ProductInfo.txt","r");

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
                if (buyerList[rowCount].uuid == idToValidate) {return false; };
                rowCount++;
            }
            return true;
        }
        return false; // returns false if a problem or if ID is not present
    } else if (strstr(dbName, "[SELLER]") != NULL) {
        struct csvClientInfo sellerList[maxRowsInDB];
        if (loadClientInfo("CustomerInfo.txt", sellerList)) {
            while (sellerList[rowCount].uuid != 0) {
                if (sellerList[rowCount].uuid == idToValidate) { return false; };
                rowCount++;
            }
            return true;
        }
        return false;
    } else if (strstr(dbName, "[PRODUCT]") != NULL) {
        struct csvProductInfo productList[maxRowsInDB];
        if (loadProductInfo(productList)) {
            while (productList[rowCount].productId != 0) {
                if (productList[rowCount].productId == idToValidate) { return false; };
                rowCount++;
            }
            return true;
        }
        return false;
    } else if (strstr(dbName, "[BILLING]") != NULL) {
        struct csvBillingInfo billingList[maxRowsInDB];
        if (loadBillingInfo(billingList)) {
            while (billingList[rowCount].orderId != 0) {
                if (billingList[rowCount].orderId == idToValidate) { return false; };
                rowCount++;
            }
            return true;
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

