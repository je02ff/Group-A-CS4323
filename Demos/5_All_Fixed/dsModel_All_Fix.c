#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

/*----Semaphores----*/
#define SEM_WRT_PRODUCT "/wrtProductInfo"
#define SEM_WRT_BILLING "/wrtBillingInfo"
#define SEM_WRT_SELLER "/wrtSellerInfo"
#define SEM_WRT_BUYER "/wrtCustomerInfo"
#define SEM_WRT_ORDER "/wrtCustomerOrderInfo"

#define SEM_READ_PRODUCT "/readProductInfo"
#define SEM_READ_BILLING "/readBillingInfo"
#define SEM_READ_SELLER "/readSellerInfo"
#define SEM_READ_BUYER "/readCustomerInfo"
#define SEM_READ_ORDER "/readCustomerOrderInfo"


#define SEM_COUNTER_BUYER "/counterCustomerInfo"




/*----Function Declarations----*/
struct command {
    char command[20];
    char clientType[20];
    int num1;
    int num2;
    bool valid;
    bool valid2;
};
void writeProductInfo ();
void writeSellerInfo ();
void writeBillingInfo ();
void writeCustomerOrderInfo();
void writeBuyerInfo();
void readProductInfo ();
void readSellerInfo ();
void readBillingInfo ();
void readCustomerOrderInfo();
void readBuyerInfo();
void* menu(void* command);

int main() {
    /*----Initialize Semaphores----*/
    sem_unlink(SEM_WRT_PRODUCT);
    sem_unlink(SEM_WRT_BILLING);
    sem_unlink(SEM_WRT_SELLER);
    sem_unlink(SEM_WRT_BUYER);
    sem_unlink(SEM_WRT_ORDER);
    sem_unlink(SEM_READ_PRODUCT);
    sem_unlink(SEM_READ_BILLING);
    sem_unlink(SEM_READ_SELLER);
    sem_unlink(SEM_READ_BUYER);
    sem_unlink(SEM_WRT_ORDER);
    sem_unlink(SEM_COUNTER_BUYER);
    sem_t *wrtProductInfo = sem_open(SEM_WRT_PRODUCT, O_CREAT, 0660, 1);
    sem_t *wrtBillingInfo = sem_open(SEM_WRT_BILLING, O_CREAT, 0660, 1);
    sem_t *wrtCustomerInfo = sem_open(SEM_WRT_BUYER, O_CREAT, 0660, 1);
    sem_t *wrtCustomerOrder = sem_open(SEM_WRT_ORDER, O_CREAT, 0660, 1);
    sem_t *wrtSellerInfo = sem_open(SEM_WRT_SELLER, O_CREAT, 0660, 1);

    sem_t *ProductInfoMutex = sem_open(SEM_READ_PRODUCT, O_CREAT, 0660, 1);
    sem_t *BillingInfoMutex = sem_open(SEM_READ_BILLING, O_CREAT, 0660, 1);
    sem_t *CustomerInfoMutex = sem_open(SEM_READ_BUYER, O_CREAT, 0660, 1);
    sem_t *CustomerOrderMutex = sem_open(SEM_READ_ORDER, O_CREAT, 0660, 1);
    sem_t *SellerInfoMutex = sem_open(SEM_READ_SELLER, O_CREAT, 0660, 1);

    sem_t *CustomerInfoCounter = sem_open(SEM_COUNTER_BUYER, O_CREAT, 0660, 0);
    int* buyerReaderCounter;

    //Setup com for desired Action
    struct command com;
    bzero(com.command,20);
    strcpy(com.command, "[NEW_CLIENT]");
    bzero(com.clientType,20);
    strcpy(com.clientType, "[BUYER]");


    char temp[30] = {0};

    while(1) {
        if (strstr(com.command, "[NEW_CLIENT]") != NULL) {
            if (strstr(com.clientType, "[BUYER]")) {
                printf("Writer is waiting for CustomerInfo\n");
                sem_wait(wrtCustomerInfo);
                printf("Writer is writing to CustomerInfo\n");
                readBuyerInfo();
                writeBuyerInfo();
                sem_post(wrtCustomerInfo);
                printf("Writer releases lock\n");
            } else {
                printf("Writer is waiting for SellerInfo\n");
                sem_wait(wrtSellerInfo);
                printf("Writer is writing to SellerInfo\n");
                readSellerInfo();
                writeSellerInfo();
                sem_post(wrtSellerInfo);
                printf("Writer releases lock\n");
            }

        } else if (strstr(com.command, "[VALIDATE_ID]") != NULL) {
            if (strstr(com.clientType, "[BUYER]")) {
                readBuyerInfo();
            } else if (strstr(com.clientType, "[SELLER]")) {
                readSellerInfo();
            } else if (strstr(com.clientType, "[PRODUCT]")) {
                readProductInfo();
            } else if (strstr(com.clientType, "[BILLING]")) {
                readBillingInfo();
            } else if (strstr(com.clientType, "[ORDER]")) {
                readCustomerOrderInfo();
            }
        } else if (strstr(com.command, "[VIEW_PRODUCTS]") != NULL) {
            if (strstr(com.clientType, "[BUYER]")) {
                readProductInfo();
            } else if (strstr(com.clientType, "[SELLER]")) {
                readProductInfo();
            }

        } else if (strstr(com.command, "[COMPLETE_ORDER]") != NULL) {
            readProductInfo();
            if (com.valid) {
                readBuyerInfo();
                readBillingInfo();
                readCustomerOrderInfo();
                writeProductInfo();
                writeBillingInfo();
                writeCustomerOrderInfo();
            }

        } else if (strstr(com.command, "[VIEW_BUYER_ORDERS]") != NULL) {
            readBillingInfo();

        } else if (strstr(com.command, "[CHECK_BUYER]") != NULL) {
            readBillingInfo();

        } else if (strstr(com.command, "[ORDER_DETAILS]") != NULL) {
            readCustomerOrderInfo();
            readProductInfo();

        } else if (strstr(com.command, "[MOD_ORDER]") != NULL) {
            readCustomerOrderInfo();
            readBillingInfo();
            writeCustomerOrderInfo();
            writeBillingInfo();

        } else if (strstr(com.command, "[VIEW_BILLING]") != NULL) {
            sem_wait(CustomerInfoMutex);
            printf("Reader has lock\n");
            sem_post(CustomerInfoCounter);
            sem_getvalue(CustomerInfoCounter, buyerReaderCounter);
            if(*buyerReaderCounter == 1) {
                sem_wait(wrtCustomerInfo);
            }
            sem_post(CustomerInfoMutex);
            printf("reader lock returned\n");
            readBuyerInfo();
            sem_wait(CustomerInfoMutex);
            sem_trywait(CustomerInfoCounter);
            sem_getvalue(CustomerInfoCounter, buyerReaderCounter);
            if(*buyerReaderCounter== 0 ) {
                sem_post(wrtCustomerInfo);
            }
            sem_post(CustomerInfoMutex);

        } else if (strstr(com.command, "[EDIT_INFO]") != NULL) {
            if (strstr(com.clientType, "[BUYER]")) {
                if (strstr(com.clientType, "[NAME]")) {
                    readBuyerInfo();
                    writeBuyerInfo();

                    readBillingInfo();
                    writeBillingInfo();

                    readCustomerOrderInfo();
                    writeCustomerOrderInfo();

                } else if (strstr(com.command, "[NUMBER]") != NULL) {
                    readBuyerInfo();
                    writeBuyerInfo();

                } else if (strstr(com.command, "[ADDRESS]") != NULL) {
                    readBuyerInfo();
                    writeBuyerInfo();

                    readBillingInfo();
                    writeBillingInfo();

                    readCustomerOrderInfo();
                    writeCustomerOrderInfo();
                }
            } else {
                if (strstr(com.clientType, "[NAME]")) {
                    readSellerInfo();
                    writeSellerInfo();

                } else if (strstr(com.command, "[NUMBER]") != NULL) {
                    readSellerInfo();
                    writeSellerInfo();
                } else if (strstr(com.command, "[ADDRESS]") != NULL) {
                    readSellerInfo();
                    writeSellerInfo();
                }
            }

        } else if (strstr(com.command, "[NEW_PRODUCT]") != NULL) {
            readProductInfo();
            writeProductInfo();

        } else if (strstr(com.command, "[DELETE_PROD]") != NULL) {
            readProductInfo();
            if (com.valid) {
                readProductInfo();
                if (com.valid2) {
                    //TODO printf("%d", globalArray); //READ global array (deleted product list)
                    //TODO globalArray = rand(); //write global array (deleted product list)
                    writeProductInfo();
                }
            }
        } else if (strstr(com.command, "[MOD_QUANT]") != NULL) {
            readProductInfo();
            if (com.valid) {
                readProductInfo();
                if (com.valid2) {
                    writeProductInfo();
                }
            }
        } else if (strstr(com.command, "[MOD_PRICE]") != NULL) {
            readProductInfo();
            if (com.valid) {
                readProductInfo();
                if (com.valid2) {
                    writeProductInfo();
                }
            }
        } else if (strstr(com.command, "[SELLER_ORDERS]") != NULL) {
            readProductInfo();
            readCustomerOrderInfo();
        }
    }
    return 0;
}

void writeProductInfo () {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    FILE*  fl1 = fopen("./txt/ProductInfo.txt","w");
    fprintf(fl1, "%s", temp);
    fclose(fl1);
    printf("%s\n",temp);
    sleep(1);
}
void writeSellerInfo () {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    FILE*  fl1 = fopen("./txt/SellerInfo.txt","w");
    fprintf(fl1, "%s", temp);
    fclose(fl1);
    printf("%s\n",temp);
    sleep(1);
}

void writeBillingInfo () {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    FILE*  fl1 = fopen("./txt/BillingInfo.txt","w");
    fprintf(fl1, "%s", temp);
    fclose(fl1);
    printf("%s\n",temp);
    sleep(1);
}

void writeCustomerOrderInfo() {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    FILE*  fl1 = fopen("./txt/CustomerOrder.txt","w");
    fprintf(fl1, "%s", temp);
    fclose(fl1);
    printf("%s\n",temp);
    sleep(1);
}

void writeBuyerInfo() {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    FILE*  fl1 = fopen("./txt/CustomerInfo.txt","w");
    fprintf(fl1, "%s", temp);
    fclose(fl1);
    printf("%s\n",temp);
    sleep(1);
}

void readProductInfo () {
    int x;
    FILE* fl1 = fopen("./txt/ProductInfo.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("%d\n", x);
    sleep(1);
}

void readSellerInfo () {
    int x;
    FILE* fl1 = fopen("./txt/SellerInfo.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("%d\n", x);
    sleep(1);
}

void readBillingInfo () {
    int x;
    FILE* fl1 = fopen("./txt/BillingInfo.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("%d\n", x);
    sleep(1);
}

void readCustomerOrderInfo() {
    int x;
    FILE* fl1 = fopen("./txt/CustomerOrder.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("%d\n", x);
    sleep(1);
}

void readBuyerInfo() {
    int x;
    FILE* fl1 = fopen("./txt/CustomerInfo.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("%d\n", x);
    sleep(1);
}


#pragma clang diagnostic pop
#pragma clang diagnostic pop