#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

/*----"Databases"----*/
char BillingInfo[30] = "Billing\n";
char CustomerInfo[30] = "Buyer\n";
char SellerInfo[30] = "Seller\n";
char CustomerOrderInfo[30] = "Orders\n";
char ProductInfo[30] = "Products\n";
int globalArray = 1;

/*----Semaphores----*/
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;
sem_t *wrtProductInfo;

struct command {
    char command[20];
    char clientType[20];
    int num1;
    int num2;
    bool valid;
    bool valid2;
};

void writeProductInfo () {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    strcat(temp,"ProductInfo\n");
    strcpy(ProductInfo, temp);
}
void writeSellerInfo () {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    strcat(temp,"Seller\n");
    strcpy(SellerInfo, temp);
}

void writeBillingInfo () {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    strcat(temp,"BillingInfo\n");
    strcpy(BillingInfo, temp);
}

void writeCustomerOrderInfo() {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    strcat(temp,"CustomerOrder\n");
    strcpy(CustomerOrderInfo, temp);
}

void writeBuyerInfo() {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    strcat(temp,"Buyer\n");
    strcpy(CustomerInfo, temp);
}

void readProductInfo () {
    printf("%s",ProductInfo);
}

void readSellerInfo () {
    printf("%s",SellerInfo);
}

void readBillingInfo () {
    printf("%s",BillingInfo);
}

void readCustomerOrderInfo() {
    printf("%s",CustomerOrderInfo);
}

void readBuyerInfo() {
    printf("%s",CustomerInfo);
}

void* menu(void* command) {
    char temp[30] = {0};
    struct command *com = (struct command *) command;
    while(1) {
        if (strstr(com->command, "[NEW_CLIENT]") != NULL) {
            if (strstr(com->clientType, "[BUYER]")) {
                readBuyerInfo();
                writeBuyerInfo();
            } else {
                readSellerInfo();
                writeSellerInfo();
            }

        } else if (strstr(com->command, "[VALIDATE_ID]") != NULL) {
            if (strstr(com->clientType, "[BUYER]")) {
                readBuyerInfo();
            } else if (strstr(com->clientType, "[SELLER]")) {
                readSellerInfo();
            } else if (strstr(com->clientType, "[PRODUCT]")) {
                readProductInfo();
            } else if (strstr(com->clientType, "[BILLING]")) {
                readBillingInfo();
            } else if (strstr(com->clientType, "[ORDER]")) {
                readCustomerOrderInfo();
            }
        } else if (strstr(com->command, "[VIEW_PRODUCTS]") != NULL) {
            if (strstr(com->clientType, "[BUYER]")) {
                readProductInfo();
            } else if (strstr(com->clientType, "[SELLER]")) {
                readProductInfo();
            }

        } else if (strstr(com->command, "[COMPLETE_ORDER]") != NULL) {
            readProductInfo();
            if (com->valid) {
                readBuyerInfo();
                readBillingInfo();
                readCustomerOrderInfo();
                writeProductInfo();
                writeBillingInfo();
                writeCustomerOrderInfo();
            }

        } else if (strstr(com->command, "[VIEW_BUYER_ORDERS]") != NULL) {
            readBillingInfo();

        } else if (strstr(com->command, "[CHECK_BUYER]") != NULL) {
            readBillingInfo();

        } else if (strstr(com->command, "[ORDER_DETAILS]") != NULL) {
            readCustomerOrderInfo();
            readProductInfo();

        } else if (strstr(com->command, "[MOD_ORDER]") != NULL) {
            readCustomerOrderInfo();
            readBillingInfo();
            writeCustomerOrderInfo();
            writeBillingInfo();

        } else if (strstr(com->command, "[VIEW_BILLING]") != NULL) {
            readBuyerInfo();

        } else if (strstr(com->command, "[EDIT_INFO]") != NULL) {
            if (strstr(com->clientType, "[BUYER]")) {
                if (strstr(com->clientType, "[NAME]")) {
                    readBuyerInfo();
                    writeBuyerInfo();

                    readBillingInfo();
                    writeBillingInfo();

                    readCustomerOrderInfo();
                    writeCustomerOrderInfo();

                } else if (strstr(com->command, "[NUMBER]") != NULL) {
                    readBuyerInfo();
                    writeBuyerInfo();

                } else if (strstr(com->command, "[ADDRESS]") != NULL) {
                    readBuyerInfo();
                    writeBuyerInfo();

                    readBillingInfo();
                    writeBillingInfo();

                    readCustomerOrderInfo();
                    writeCustomerOrderInfo();
                }
            } else {
                if (strstr(com->clientType, "[NAME]")) {
                    readSellerInfo();
                    writeSellerInfo();

                } else if (strstr(com->command, "[NUMBER]") != NULL) {
                    readSellerInfo();
                    writeSellerInfo();
                } else if (strstr(com->command, "[ADDRESS]") != NULL) {
                    readSellerInfo();
                    writeSellerInfo();
                }
            }

        } else if (strstr(com->command, "[NEW_PRODUCT]") != NULL) {
            readProductInfo();
            writeProductInfo();

        } else if (strstr(com->command, "[DELETE_PROD]") != NULL) {
            readProductInfo();
            if (com->valid) {
                readProductInfo();
                if (com->valid2) {
                    printf("%d", globalArray); //READ global array (deleted product list)
                    globalArray = rand(); //write global array (deleted product list)
                    writeProductInfo();
                }
            }
        } else if (strstr(com->command, "[MOD_QUANT]") != NULL) {
            readProductInfo();
            if (com->valid) {
                readProductInfo();
                if (com->valid2) {
                    writeProductInfo();
                }
            }
        } else if (strstr(com->command, "[MOD_PRICE]") != NULL) {
            readProductInfo();
            if (com->valid) {
                readProductInfo();
                if (com->valid2) {
                    writeProductInfo();
                }
            }
        } else if (strstr(com->command, "[SELLER_ORDERS]") != NULL) {
            readProductInfo();
            readCustomerOrderInfo();
        }
    }
    return NULL;
}


int main() {
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;

    //Setup com1 to pass to thread1
    struct command com1;
    bzero(com1.command,20);
    strcpy(com1.command, "[NEW_CLIENT]");
    bzero(com1.clientType,20);
    strcpy(com1.clientType, "[BUYER]");

    //Setup com2 to pass to thread2
    struct command com2;
    bzero(com2.command,20);
    strcpy(com2.command, "[VALIDATE_ID]");
    bzero(com2.clientType,20);
    strcpy(com2.clientType, "[BUYER]");


    //Setup com3 to pass to thread3
    struct command com3;
    bzero(com3.command,20);
    strcpy(com3.command, "[NEW_CLIENT]");
    bzero(com3.clientType,20);
    strcpy(com3.clientType, "[BUYER]");


    pthread_create(&thread1,NULL, menu, &com1);
    pthread_create(&thread2,NULL, menu, &com2);
    //pthread_create(&thread3,NULL, menu, &com3);




    while(1){};
    return 0;
}
#pragma clang diagnostic pop
#pragma clang diagnostic pop