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
#include "Roden.h"

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

                    if(strstr(command, "[NEW_CLIENT]") != NULL) {
                        /*1.store new client data  TCP COMMAND: [NEW_CLIENT]*/
                        //REQUIRED BUFFER STRING: "[NEW_CLIENT],[BUYER],FirstName,LastName,phoneNumber,streetAddress,City,State,zipcode,"  <--notice buffer should end with ','
                        //OR REQUIRED BUFFER STRING: "[NEW_CLIENT],[SELLER],FirstName,LastName,phoneNumber,streetAddress,City,State,zipcode,"

                        writeNewClientData(buffer);              //send modified buffer to extract contents and write to ConsumerInfo or SellerInfo.txt
                        writeSocket(&clientSock, "[CONFIRMATION]"); //Send confirmation to clientServer that info was added
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    } else if(strstr(command, "[VALIDATE_ID]") != NULL) {
                        /*2.Validate an ID in a database  TCP COMMAND: [VALIDATE_ID]*/
                        /*Required Buffer string: "[VALIDATE_ID],3432,[BUYER]"
                        or "[VALIDATE_ID],numberID,[SELLER]"
                        or "[VALIDATE_ID],numberID,[PRODUCT]"
                        or "[VALIDATE_ID],numberID,[BILLING]"          */

                        validateID(buffer);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

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


                    } else if(strstr(command, "[VIEW_PRODUCTS]") != NULL) {
                        /*3. View Available Products TCP COMMAND: [VIEW_PRODUCTS]*/
                        //REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[BUYER],"
                        //OR REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[SELLER],uuid,"

                        viewProducts(buffer, &clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    } else if(strstr(command, "[COMPLETE_ORDER]") != NULL) {
                        /*4. BuyerOPTION 1-3 Complete Order TCP COMMAND: [COMPLETE_ORDER]*/
                        //Required Buffer String: "[COMPLETE_ORDER],int buyerID,int productID,int quantityOrdered,int pricePerUnit,int productID,int quantityOrdered,int pricePerUnit, ...,"

                        completeAnOrder(buffer,&clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    } else if(strstr(command, "[VIEW_BUYER_ORDERS]") != NULL) {
                        /*5. BuyerOPTION 2 View Orders TCP COMMAND: [VIEW_BUYER_ORDERS]*/
                        //Required Buffer String: "[VIEW_BUYER_ORDERS],int buyerID,"

                        buyerViewsOrder(buffer, &clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);


                    } else if(strstr(command, "[MOD_ORDER]") != NULL) {
                        /*6. BuyerOPTION 3 Modify Order TCP COMMAND: [MOD_ORDER]*/
                        //TODO

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



                    } else if(strstr(command, "[VIEW_BILLING]") != NULL) {
                        /*7. BuyerOPTION 4 View Billing Info TCP COMMAND: [VIEW_BILLING]*/
                        //Required Buffer String: "[VIEW_BILLING],int buyerID,"

                        buyerViewsInfo(buffer, &clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);


                    } else if(strstr(command, "[EDIT_INFO]") != NULL) {
                        /*8. BuyerOPTION/SellerOPTION 5 Edit Info TCP COMMAND: [EDIT_INFO]*/
                        //TODO

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



                    } else if(strstr(command, "[NEW_PRODUCT]") != NULL) {
                        /*9. SellerOPTION 2 Add New Product TCP COMMAND: [NEW_PRODUCT]*/
                        //REQ Buffer string: "[NEW_PRODUCT],sellerID,productName,quantityAvailable,pricePerUnit,"

                        addNewProduct(buffer, &clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    } else if(strstr(command, "[DELETE_PROD]") != NULL) {
                        /*10. SellerOption 3 Delete Product TCP COMMAND: [DELETE_PROD]*/
                        //REQUIRED Buff string: "[DELETE_PROD],sellerID,productID,"

                        deleteProduct(buffer, &clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    } else if(strstr(command, "[MOD_QUANT]") != NULL) {
                        /*11. SellerOption 4 Modify Product Quantity TCP COMMAND: [MOD_QUANT]*/
                        //REQ Buffer string: "[MOD_QUANT],sellerID,productID,quantityToSet"

                        modifyQuantity(buffer, &clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    } else if(strstr(command, "[MOD_PRICE]") != NULL) {
                        /*12. SellerOption 5 Modify Product Price TCP COMMAND: [MOD_PRICE]*/
                        //REQ Buffer string: "[MOD_PRICE],sellerID,productID,priceToSet"

                        modifyPrice(buffer, &clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    } else if(strstr(command, "[SELLER_ORDERS]") != NULL) {
                        /*13. SellerOption 6 View Product Orders TCP COMMAND: [SELLER_ORDERS]*/
                        //TODO

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

