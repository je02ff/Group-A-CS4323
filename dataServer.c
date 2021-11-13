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
    int deletedProducts[maxRowsInDB] = {0};
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

                        writeNewClientData(buffer, &clientSock);
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                        //SENDS BACK: newClientID

                    } else if(strstr(command, "[VALIDATE_ID]") != NULL) {
                        /*2.Validate an ID in a database  TCP COMMAND: [VALIDATE_ID]*/
                        /*Required Buffer string: "buyerID,[BUYER],"
                                                or "sellerID,[SELLER],"
                                                or "productID,[PRODUCT],"
                                                or "orderID,[BILLING],"
                                                or "productID,[ORDER],"          */
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

                        deleteProduct(buffer, &clientSock, deletedProducts);
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

