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

#define PORT_NUM 8080
#define CLIENT_BACKLOG 25
#define MSG_BUFFER_SIZE 10240

/*---Function Declarations---*/
void createSocket(int *sock);

void bindSocket(struct sockaddr_in *sAddress, const int *sock);

void listenOnSocket(const int* sock, int backLogCount);

bool acceptConnection(int* clientSock, const int* hSock, struct sockaddr_in* client);

void readSocket(const int* sock, char* buffer);

void writeSocket(const int* sock, char* buffer);

void populateNewClientData(bool isBuyer, int* clientSock, int* clientID);

bool validateID(char* buffer, char* dataBase, int* clientSock);

int main() {
    /* TCP Vars */
    int hSocket;
    int clientSock;
    struct sockaddr_in server = {0};
    struct sockaddr_in client = {0};
    char buffer[MSG_BUFFER_SIZE];
    pid_t pid;

    createSocket(&hSocket);                                             //Creating socket for server to communicate through
    bindSocket(&server, &hSocket);                                      // binding socket to address
    listenOnSocket(&hSocket, CLIENT_BACKLOG);                           //Listen for clients, maximum of 3 connections


    /*----Forking Clients----*/
    while (1) {
        if (acceptConnection(&clientSock, &hSocket, &client) == true) { //Accept incoming client connection
            pid = fork();                                               //Fork client process
            if(pid == 0) {                                              //code for child(client) process
                close(hSocket);                                       //close hSocket, child uses clientSock
                bool isBuyer = true;                                    //indicates if client is a buyer or seller
                bool clientIDSet = false;
                int clientID;

                /*---Establish if client is a Buyer or a Seller---*/
                bzero(buffer, MSG_BUFFER_SIZE);
                readSocket(&clientSock, buffer);
                if(strstr(buffer, "[BUYER]") != NULL) {
                    isBuyer = true;
                } else if (strstr(buffer, "[SELLER]") != NULL) {
                    isBuyer = false;
                }
                bzero(buffer, MSG_BUFFER_SIZE);

                /*---Set clientID ---*/
                while(!clientIDSet) {
                    readSocket(&clientSock, buffer);
                    if(strstr(buffer, "[NEW_CLIENT]") != NULL) {
                        populateNewClientData(isBuyer, &clientSock, &clientID);
                        writeSocket(&clientSock, "[CONFIRMATION]");
                        clientIDSet = true;
                    } else if (strstr(buffer, "[LOGIN]") != NULL) {
                        bzero(buffer, MSG_BUFFER_SIZE);
                        readSocket(&clientSock, buffer); //read UID sent by Client
                        if(isBuyer) {
                            if (validateID(buffer, "CustomerInformation.txt", &clientSock)) {
                                clientID = (int) buffer;
                                clientIDSet = true;
                            }
                        } else {
                            if (validateID(buffer, "SellerInformation.txt", &clientSock)) {
                                clientID = (int) buffer;
                                clientIDSet = true;
                            }
                        }
                    }
                }

                /*---Main Logic Loop: Driven by Client Commands ---*/
                while(strstr(buffer, "[QUIT]") != NULL) {
                    bzero(buffer, MSG_BUFFER_SIZE);
                    readSocket(&clientSock, buffer);

                    //more code commands
                }
                /*---Client Quit ---*/
                    close(clientSock);
                    exit(1);


                //Instructions a client can make:
                 /* On client start, SEND [BUYER] if user select Buyer Menu or [SELLER] if user selects Seller Menu

                    1.store new client data  TCP COMMAND: [NEW_CLIENT]
                        IF new Buyer
                            WRITE CustomerInformation.txt
                                NEW CustomerID, FirstName, LastName, phoneNumber, streetAddress, City, State, zipcode
                            SEND Confirmation
                        IF new Seller
                            WRITE SellerInformation.txt
                                NEW SellerID, FirstName, LastName, phoneNumber, streetAddress, City, State, zipcode
                            SEND Confirmation

                    2. bool validateID(int ID, string DatabaseName) TCP COMMAND: [VALIDATE_ID]
                            if ID in DatabaseName
                                return true
                            else
                                return false

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



                    3. View Available Products TCP COMMAND: [VIEW_PRODUCTS]
                        IF Buyer
                            READ ProductInfo.txt
                            SEND ALL: ProductIDs, Product Names, Quantitys, Price/Units

                        IF Seller
                            READ ProductInfo.txt
                            SEND FILTERED to Seller's ID:  ProductIDs, Product Names, Quantitys, Price/Units

                    4. BuyerOPTION 1-3 Complete Order TCP COMMAND: [COMPLETE_ORDER]
                        READ ProductInfo.txt validate given ProductID's AND Quantities
                        //validateID(ProductID, "ProductInfo.txt")
                        //validateQuantity(int ProductID, int quantity)

                        IF OK --> WRITE CustomerOrder.txt AND WRITE BillingInfo.txt
                            SEND Order Conformation
                        ELSE Invalid
                            SEND Order Invalid

                    5. BuyerOPTION 2 View Orders TCP COMMAND: [VIEW_BUYER_ORDERS]
                        READ BillingInfo.txt
                            SEND OrderID, Total Order Price

                    6. BuyerOPTION 3 Modify Order TCP COMMAND: [MOD_ORDER]
                        OrderID -- present in READ BillingInfo.txt //validateID(OrderID, "BillingInfo.txt")
                        IF OK --> READ CustomerOrder.txt AND READ BillingInfo.txt
                            SEND ProductID, Product Name, quantity, Total Price

                            ClientOPTION 3-1 Return a product
                                productID present in READ CustomerOrder.txt //validateID(OrderID, ProductID, "CustomerOrder.txt") NEED TO OVERLOAD THIS FUNCTION FOR IT TO WORK
                                IF OK --> WRITE CustomerOrder.txt AND WRITE BillingInfo.txt AND WRITE ProductInfo.txt
                                    Send Order Modified
                                ELSE
                                    SEND Invalid
                        ELSE Invalid
                            SEND Order Invalid

                    7. BuyerOPTION 4 View Billing Info TCP COMMAND: [VIEW_BILLING]
                            READ CustomerInfo.txt
                            SEND Name, Number, Address

                    8. BuyerOPTION/SellerOPTION 5 Edit Info TCP COMMAND: [EDIT_INFO]
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
                                    SEND Confirmation

                    9. SellerOPTION 2 Add New Product TCP COMMAND: [NEW_PRODUCT]
                            WRITE ProductInfo.txt
                                new ProductID, productName, SellerID, Quantity, Price/Unit
                            SEND Confirmation

                    10. SellerOption 3 Delete Product TCP COMMAND: [DELETE_PROD]
                            READ ProductInfo.txt
                            validate ProductID //validateID(productID, ProductInfo.txt) TCP COMMAND:

                            IF OK --> WRITE ProductInfo.txt AND WRITE BillingInfo.txt AND WRITE CustomerOrder.txt
                                delete from ProductInfo
                                update CustomerOrder.txt and store total item price
                                update BillingInfo.txt total Order Price
                                SEND Confirmation
                            ELSE
                                 SEND Invalid

                    11. SellerOption 4 Modify Product Quantity TCP COMMAND: [MOD_QUANT]
                            READ ProductInfo.txt
                            validate ProductID //validateID(productID, "ProductInfo.txt")

                            IF OK --> WRITE ProductInfo.txt
                                SEND Confirmation
                            ELSE
                                SEND InvalidID

                    12. SellerOption 5 Modify Product Price TCP COMMAND: [MOD_PRICE]
                            READ ProductInfo.txt
                            validate ProductID //validateID(productID, ProductInfo.txt)

                            IF OK --> WRITE ProductInfo.txt
                                adjust Product Price
                                SEND Confirmation
                            ELSE
                                SEND Invalid ID

                    13. SellerOption 6 View Product Orders TCP COMMAND: [SELLER_ORDERS]
                            READ ProductInfo.txt
                                get ProductIDList for seller's ID

                            READ CustomerOrder.txt
                            SEND Filtered to ProductIDList: OrderID,ProductID, Quantity, Total Price, Delivery Address


                 */


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

/*---Function Definitions---*/

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
    sAddress->sin_port = htons(PORT_NUM);
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
        printf("Listening through socket %d on port %d\n", *sock, PORT_NUM);
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

void populateNewClientData(bool isBuyer, int* clientSock, int* clientID) {
    char firstName[30];
    char lastName[30];
    char streetAddress[30];
    char city[30];
    char state[30];
    char zipCode[30];

    //CODE TO read data into char arrays. generate new UID and set to clientID
    // and store into either CustomerInformation.txt or SellerInformation.txt


}


bool validateID(char* buffer, char* dataBase, int* clientSock) {

}