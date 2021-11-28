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
#define SEM_COUNTER_SELLER "/counterSellerInfo"
#define SEM_COUNTER_BILLING "/counterBillingInfo"
#define SEM_COUNTER_ORDER "/counterOrderInfo"
#define SEM_COUNTER_PRODUCT "/counterProductInfo"




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

int main() {
    /*----Initialize Semaphores----*/
    //Unlink any semaphores from previous tests, should only be active in the first process to begin demo
   /* sem_unlink(SEM_WRT_PRODUCT);
    sem_unlink(SEM_WRT_BILLING);
    sem_unlink(SEM_WRT_SELLER);
    sem_unlink(SEM_WRT_BUYER);
    sem_unlink(SEM_WRT_ORDER);

    sem_unlink(SEM_READ_PRODUCT);
    sem_unlink(SEM_READ_BILLING);
    sem_unlink(SEM_READ_SELLER);
    sem_unlink(SEM_READ_BUYER);
    sem_unlink(SEM_READ_ORDER);

    sem_unlink(SEM_COUNTER_BUYER);
    sem_unlink(SEM_COUNTER_SELLER);
    sem_unlink(SEM_COUNTER_BILLING);
    sem_unlink(SEM_COUNTER_PRODUCT);
    sem_unlink(SEM_COUNTER_ORDER);*/

    //binary semaphores to lock writing priveledges
    sem_t *wrtProductInfo = sem_open(SEM_WRT_PRODUCT, O_CREAT, 0660, 1);
    sem_t *wrtBillingInfo = sem_open(SEM_WRT_BILLING, O_CREAT, 0660, 1);
    sem_t *wrtCustomerInfo = sem_open(SEM_WRT_BUYER, O_CREAT, 0660, 1);
    sem_t *wrtCustomerOrder = sem_open(SEM_WRT_ORDER, O_CREAT, 0660, 1);
    sem_t *wrtSellerInfo = sem_open(SEM_WRT_SELLER, O_CREAT, 0660, 1);

    //binary semaphores to lock modifying the number of readers reading(counters)
    sem_t *ProductInfoMutex = sem_open(SEM_READ_PRODUCT, O_CREAT, 0660, 1);
    sem_t *BillingInfoMutex = sem_open(SEM_READ_BILLING, O_CREAT, 0660, 1);
    sem_t *CustomerInfoMutex = sem_open(SEM_READ_BUYER, O_CREAT, 0660, 1);
    sem_t *CustomerOrderMutex = sem_open(SEM_READ_ORDER, O_CREAT, 0660, 1);
    sem_t *SellerInfoMutex = sem_open(SEM_READ_SELLER, O_CREAT, 0660, 1);

    //counting semaphores to count the number of readers reading a file. (sem_t counts, int* points to the sem's value)
    sem_t *CustomerInfoCounter = sem_open(SEM_COUNTER_BUYER, O_CREAT, 0660, 0);
    int buyerReaderCounter;
    sem_getvalue(CustomerInfoCounter, &buyerReaderCounter);

    sem_t *SellerInfoCounter = sem_open(SEM_COUNTER_SELLER, O_CREAT, 0660, 0);
    int sellerReaderCounter;
    sem_getvalue(SellerInfoCounter, &sellerReaderCounter);

    sem_t *BillingInfoCounter = sem_open(SEM_COUNTER_BILLING, O_CREAT, 0660, 0);
    int billingReaderCounter;
    sem_getvalue(BillingInfoCounter, &billingReaderCounter);

    sem_t *ProductInfoCounter = sem_open(SEM_COUNTER_PRODUCT, O_CREAT, 0660, 0);
    int productReaderCounter;
    sem_getvalue(ProductInfoCounter, &productReaderCounter);

    sem_t *OrderInfoCounter = sem_open(SEM_COUNTER_ORDER, O_CREAT, 0660, 0);
    int orderReaderCounter;
    sem_getvalue(OrderInfoCounter, &orderReaderCounter);

    //Setup com for desired Action
    struct command com;
    bzero(com.command,20);
    strcpy(com.command, "[VALIDATE_ID]");
    bzero(com.clientType,20);
    strcpy(com.clientType, "[PRODUCT]");


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
                sem_wait(CustomerInfoMutex);
                printf("Reader has counter lock\n");
                sem_post(CustomerInfoCounter);
                sem_getvalue(CustomerInfoCounter, &buyerReaderCounter);
                if(buyerReaderCounter == 1) {
                    sem_wait(wrtCustomerInfo);
                }
                sem_post(CustomerInfoMutex);
                printf("reader counter lock returned\n");
                readBuyerInfo();
                sem_wait(CustomerInfoMutex);
                sem_trywait(CustomerInfoCounter);
                sem_getvalue(CustomerInfoCounter, &buyerReaderCounter);
                if(buyerReaderCounter== 0 ) {
                    sem_post(wrtCustomerInfo);
                }
                sem_post(CustomerInfoMutex);
            } else if (strstr(com.clientType, "[SELLER]")) {
                sem_wait(SellerInfoMutex);
                printf("Reader has counter lock\n");
                sem_post(SellerInfoCounter);
                sem_getvalue(SellerInfoCounter, &sellerReaderCounter);
                if(sellerReaderCounter == 1) {
                    sem_wait(wrtSellerInfo);
                }
                sem_post(SellerInfoMutex);
                printf("reader counter lock returned\n");
                readSellerInfo();
                sem_wait(SellerInfoMutex);
                sem_trywait(SellerInfoCounter);
                sem_getvalue(SellerInfoCounter, &sellerReaderCounter);
                if(sellerReaderCounter== 0 ) {
                    sem_post(wrtSellerInfo);
                }
                sem_post(SellerInfoMutex);
            } else if (strstr(com.clientType, "[PRODUCT]")) {
                sem_wait(ProductInfoMutex);
                printf("Reader has counter lock\n");
                sem_post(ProductInfoCounter);
                sem_getvalue(ProductInfoCounter, &productReaderCounter);
                if(productReaderCounter == 1) {
                    sem_wait(wrtProductInfo);
                }
                sem_post(ProductInfoMutex);
                printf("reader counter lock returned\n");
                readProductInfo();
                sem_wait(ProductInfoMutex);
                sem_trywait(ProductInfoCounter);
                sem_getvalue(ProductInfoCounter, &productReaderCounter);
                if(productReaderCounter == 0 ) {
                    sem_post(wrtProductInfo);
                }
                sem_post(ProductInfoMutex);
            } else if (strstr(com.clientType, "[BILLING]")) {
                sem_wait(BillingInfoMutex);
                printf("Reader has counter lock\n");
                sem_post(BillingInfoCounter);
                sem_getvalue(BillingInfoCounter, &billingReaderCounter);
                if(billingReaderCounter == 1) {
                    sem_wait(wrtBillingInfo);
                }
                sem_post(BillingInfoMutex);
                printf("reader counter lock returned\n");
                readBillingInfo();
                sem_wait(BillingInfoMutex);
                sem_trywait(BillingInfoCounter);
                sem_getvalue(BillingInfoCounter, &billingReaderCounter);
                if(billingReaderCounter== 0 ) {
                    sem_post(wrtBillingInfo);
                }
                sem_post(BillingInfoMutex);
            } else if (strstr(com.clientType, "[ORDER]")) {
                sem_wait(CustomerOrderMutex);
                printf("Reader has counter lock\n");
                sem_post(OrderInfoCounter);
                sem_getvalue(OrderInfoCounter, &orderReaderCounter);
                if(orderReaderCounter == 1) {
                    sem_wait(wrtCustomerOrder);
                }
                sem_post(CustomerOrderMutex);
                printf("reader counter lock returned\n");
                readCustomerOrderInfo();
                sem_wait(CustomerOrderMutex);
                sem_trywait(OrderInfoCounter);
                sem_getvalue(OrderInfoCounter, &orderReaderCounter);
                if(orderReaderCounter== 0 ) {
                    sem_post(wrtCustomerOrder);
                }
                sem_post(CustomerOrderMutex);
            }
        } else if (strstr(com.command, "[VIEW_PRODUCTS]") != NULL) {
            if (strstr(com.clientType, "[BUYER]")) {
                sem_wait(ProductInfoMutex);
                printf("Reader has counter lock\n");
                sem_post(ProductInfoCounter);
                sem_getvalue(ProductInfoCounter, &productReaderCounter);
                if(productReaderCounter == 1) {
                    sem_wait(wrtProductInfo);
                }
                sem_post(ProductInfoMutex);
                printf("reader counter lock returned\n");
                readProductInfo();
                sem_wait(ProductInfoMutex);
                sem_trywait(ProductInfoCounter);
                sem_getvalue(ProductInfoCounter, &productReaderCounter);
                if(productReaderCounter == 0 ) {
                    sem_post(wrtProductInfo);
                }
                sem_post(ProductInfoMutex);
            } else if (strstr(com.clientType, "[SELLER]")) {
                sem_wait(ProductInfoMutex);
                printf("Reader has counter lock\n");
                sem_post(ProductInfoCounter);
                sem_getvalue(ProductInfoCounter, &productReaderCounter);
                if(productReaderCounter == 1) {
                    sem_wait(wrtProductInfo);
                }
                sem_post(ProductInfoMutex);
                printf("reader counter lock returned\n");
                readProductInfo();
                sem_wait(ProductInfoMutex);
                sem_trywait(ProductInfoCounter);
                sem_getvalue(ProductInfoCounter, &productReaderCounter);
                if(productReaderCounter == 0 ) {
                    sem_post(wrtProductInfo);
                }
                sem_post(ProductInfoMutex);
            }

        } else if (strstr(com.command, "[COMPLETE_ORDER]") != NULL) {
            //the following read occurs in a call to [VALIDATE_ID]
            //readProductInfo();
            if (com.valid) {
                printf("Writer is waiting for ProductInfo\n");
                sem_wait(wrtProductInfo);
                printf("Writer now waiting for BillingInfo\n");
                sem_wait(wrtBillingInfo);
                printf("Writer now waiting for OrderInfo\n");
                sem_wait(wrtCustomerOrder);

                readProductInfo();
                readBillingInfo();
                readCustomerOrderInfo();

                writeProductInfo();
                writeBillingInfo();
                writeCustomerOrderInfo();

                sem_post(wrtProductInfo);
                printf("Writer released Product lock\n");
                sem_post(wrtBillingInfo);
                printf("Writer released Billing lock\n");
                sem_post(wrtCustomerOrder);
                printf("Writer released Order lock...op finished\n");
            }

        } else if (strstr(com.command, "[VIEW_BUYER_ORDERS]") != NULL) {
            sem_wait(BillingInfoMutex);
            printf("Reader has counter lock\n");
            sem_post(BillingInfoCounter);
            sem_getvalue(BillingInfoCounter, &billingReaderCounter);
            if(billingReaderCounter == 1) {
                sem_wait(wrtBillingInfo);
            }
            sem_post(BillingInfoMutex);
            printf("reader counter lock returned\n");
            readBillingInfo();
            sem_wait(BillingInfoMutex);
            sem_trywait(BillingInfoCounter);
            sem_getvalue(BillingInfoCounter, &billingReaderCounter);
            if(billingReaderCounter== 0 ) {
                sem_post(wrtBillingInfo);
            }
            sem_post(BillingInfoMutex);

        } else if (strstr(com.command, "[CHECK_BUYER]") != NULL) {
            sem_wait(BillingInfoMutex);
            printf("Reader has counter lock\n");
            sem_post(BillingInfoCounter);
            sem_getvalue(BillingInfoCounter, &billingReaderCounter);
            if(billingReaderCounter == 1) {
                sem_wait(wrtBillingInfo);
            }
            sem_post(BillingInfoMutex);
            printf("reader counter lock returned\n");
            readBillingInfo();
            sem_wait(BillingInfoMutex);
            sem_trywait(BillingInfoCounter);
            sem_getvalue(BillingInfoCounter, &billingReaderCounter);
            if(billingReaderCounter== 0 ) {
                sem_post(wrtBillingInfo);
            }
            sem_post(BillingInfoMutex);

        } else if (strstr(com.command, "[ORDER_DETAILS]") != NULL) {
            sem_wait(CustomerOrderMutex);
            printf("Reader has counter lock\n");
            sem_post(OrderInfoCounter);
            sem_getvalue(OrderInfoCounter, &orderReaderCounter);
            if(orderReaderCounter == 1) {
                sem_wait(wrtCustomerOrder);
            }
            sem_post(CustomerOrderMutex);
            printf("reader counter lock returned\n");
            readCustomerOrderInfo();
            sem_wait(CustomerOrderMutex);
            sem_trywait(OrderInfoCounter);
            sem_getvalue(OrderInfoCounter, &orderReaderCounter);
            if(orderReaderCounter== 0 ) {
                sem_post(wrtCustomerOrder);
            }
            sem_post(CustomerOrderMutex);


            sem_wait(ProductInfoMutex);
            printf("Reader has counter lock\n");
            sem_post(ProductInfoCounter);
            sem_getvalue(ProductInfoCounter, &productReaderCounter);
            if(productReaderCounter == 1) {
                sem_wait(wrtProductInfo);
            }
            sem_post(ProductInfoMutex);
            printf("reader counter lock returned\n");
            readProductInfo();
            sem_wait(ProductInfoMutex);
            sem_trywait(ProductInfoCounter);
            sem_getvalue(ProductInfoCounter, &productReaderCounter);
            if(productReaderCounter == 0 ) {
                sem_post(wrtProductInfo);
            }
            sem_post(ProductInfoMutex);

        } else if (strstr(com.command, "[MOD_ORDER]") != NULL) {
            printf("Writer is waiting for Billing info\n");
            sem_wait(wrtBillingInfo);
            printf("Writer is waiting for Order info\n");
            sem_wait(wrtCustomerOrder);

            readCustomerOrderInfo();
            readBillingInfo();
            writeCustomerOrderInfo();
            writeBillingInfo();

            sem_post(wrtBillingInfo);
            printf("Writer released Billing lock\n");
            sem_post(wrtCustomerOrder);
            printf("Writer released Order lock\n");

        } else if (strstr(com.command, "[VIEW_BILLING]") != NULL) {
            sem_wait(CustomerInfoMutex);
            printf("Reader has counter lock\n");
            sem_post(CustomerInfoCounter);
            sem_getvalue(CustomerInfoCounter, &buyerReaderCounter);
            if(buyerReaderCounter == 1) {
                sem_wait(wrtCustomerInfo);
            }
            sem_post(CustomerInfoMutex);
            printf("reader counter lock returned\n");
            readBuyerInfo();
            sem_wait(CustomerInfoMutex);
            sem_trywait(CustomerInfoCounter);
            sem_getvalue(CustomerInfoCounter, &buyerReaderCounter);
            if(buyerReaderCounter== 0 ) {
                sem_post(wrtCustomerInfo);
            }
            sem_post(CustomerInfoMutex);

        } else if (strstr(com.command, "[EDIT_INFO]") != NULL) {
            if (strstr(com.clientType, "[BUYER]")) {
                if (strstr(com.clientType, "[NAME]")) {
                    printf("Writer is waiting for CustomerInfo\n");
                    sem_wait(wrtCustomerInfo);
                    printf("Writer is waiting for Billing Info\n");
                    sem_wait(wrtBillingInfo);
                    printf("Writer is waiting for Order Info\n");
                    sem_wait(wrtCustomerOrder);


                    readBuyerInfo();
                    writeBuyerInfo();

                    readBillingInfo();
                    writeBillingInfo();

                    readCustomerOrderInfo();
                    writeCustomerOrderInfo();

                    sem_post(wrtCustomerInfo);
                    printf("Writer released CustomerInfo lock\n");
                    sem_post(wrtBillingInfo);
                    printf("Writer released Billing lock\n");
                    sem_post(wrtCustomerOrder);
                    printf("Writer released Order lock\n");

                } else if (strstr(com.command, "[NUMBER]") != NULL) {
                    printf("Writer is waiting for CustomerInfo\n");
                    sem_wait(wrtCustomerInfo);

                    readBuyerInfo();
                    writeBuyerInfo();

                    sem_post(wrtCustomerInfo);
                    printf("Writer released CustomerInfo lock\n");

                } else if (strstr(com.command, "[ADDRESS]") != NULL) {
                    printf("Writer is waiting for CustomerInfo\n");
                    sem_wait(wrtCustomerInfo);
                    printf("Writer is waiting for Billing Info\n");
                    sem_wait(wrtBillingInfo);
                    printf("Writer is waiting for Order Info\n");
                    sem_wait(wrtCustomerOrder);


                    readBuyerInfo();
                    writeBuyerInfo();

                    readBillingInfo();
                    writeBillingInfo();

                    readCustomerOrderInfo();
                    writeCustomerOrderInfo();

                    sem_post(wrtCustomerInfo);
                    printf("Writer released CustomerInfo lock\n");
                    sem_post(wrtBillingInfo);
                    printf("Writer released Billing lock\n");
                    sem_post(wrtCustomerOrder);
                    printf("Writer released Order lock\n");
                }
            } else {
                if (strstr(com.clientType, "[NAME]")) {
                    printf("Writer is waiting for SellerInfo\n");
                    sem_wait(wrtSellerInfo);

                    readSellerInfo();
                    writeSellerInfo();

                    sem_post(wrtSellerInfo);
                    printf("Writer released SellerInfo lock\n");
                } else if (strstr(com.command, "[NUMBER]") != NULL) {
                    printf("Writer is waiting for SellerInfo\n");
                    sem_wait(wrtSellerInfo);

                    readSellerInfo();
                    writeSellerInfo();

                    sem_post(wrtSellerInfo);
                    printf("Writer released SellerInfo lock\n");
                } else if (strstr(com.command, "[ADDRESS]") != NULL) {
                    printf("Writer is waiting for SellerInfo\n");
                    sem_wait(wrtSellerInfo);

                    readSellerInfo();
                    writeSellerInfo();

                    sem_post(wrtSellerInfo);
                    printf("Writer released SellerInfo lock\n");
                }
            }

        } else if (strstr(com.command, "[NEW_PRODUCT]") != NULL) {
            printf("Writer is waiting for ProductInfo\n");
            sem_wait(wrtProductInfo);

            readProductInfo();
            writeProductInfo();

            sem_post(wrtProductInfo);
            printf("Writer released Product lock\n");

        } else if (strstr(com.command, "[DELETE_PROD]") != NULL) {
            //the following read occurs in a call to [VALIDATE_ID]
            //readProductInfo();
            if (com.valid) {
                printf("Writer is waiting for ProductInfo\n");
                sem_wait(wrtProductInfo);
                printf("Writer is waiting for deletedProductList\n");
                //sem_wait(wrtDeletedProductList);

                readProductInfo();
                if (com.valid2) {
                    //printf("%d", globalArray); //READ global array (deleted product list)
                    //globalArray = rand(); //write global array (deleted product list)
                    writeProductInfo();
                }

                sem_post(wrtProductInfo);
                printf("Writer released ProductInfo lock\n");
                //sem_post(wrtDeletedProductList);
                printf("Writer released deletedProductList lock\n");
            }
        } else if (strstr(com.command, "[MOD_QUANT]") != NULL) {
            //the following read occurs in a call to [VALIDATE_ID]
            //readProductInfo();
            if (com.valid) {
                printf("Writer is waiting for ProductInfo\n");
                sem_wait(wrtProductInfo);

                readProductInfo();
                if (com.valid2) {
                    writeProductInfo();
                }
                sem_post(wrtProductInfo);
                printf("Writer released ProductInfo lock\n");
            }
        } else if (strstr(com.command, "[MOD_PRICE]") != NULL) {
            //the following read occurs in a call to [VALIDATE_ID]
            //readProductInfo();
            if (com.valid) {
                printf("Writer is waiting for ProductInfo\n");
                sem_wait(wrtProductInfo);

                readProductInfo();
                if (com.valid2) {
                    writeProductInfo();
                }

                sem_post(wrtProductInfo);
                printf("Writer released ProductInfo lock\n");
            }
        } else if (strstr(com.command, "[SELLER_ORDERS]") != NULL) {
            sem_wait(ProductInfoMutex);
            printf("Reader has counter lock\n");
            sem_post(ProductInfoCounter);
            sem_getvalue(ProductInfoCounter, &productReaderCounter);
            if(productReaderCounter == 1) {
                sem_wait(wrtProductInfo);
            }
            sem_post(ProductInfoMutex);
            printf("reader counter lock returned\n");
            readProductInfo();
            sem_wait(ProductInfoMutex);
            sem_trywait(ProductInfoCounter);
            sem_getvalue(ProductInfoCounter, &productReaderCounter);
            if(productReaderCounter == 0 ) {
                sem_post(wrtProductInfo);
            }
            sem_post(ProductInfoMutex);

            sem_wait(CustomerOrderMutex);
            printf("Reader has counter lock\n");
            sem_post(OrderInfoCounter);
            sem_getvalue(OrderInfoCounter, &orderReaderCounter);
            if(orderReaderCounter == 1) {
                sem_wait(wrtCustomerOrder);
            }
            sem_post(CustomerOrderMutex);
            printf("reader counter lock returned\n");
            readCustomerOrderInfo();
            sem_wait(CustomerOrderMutex);
            sem_trywait(OrderInfoCounter);
            sem_getvalue(OrderInfoCounter, &orderReaderCounter);
            if(orderReaderCounter== 0 ) {
                sem_post(wrtCustomerOrder);
            }
            sem_post(CustomerOrderMutex);
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
    printf("product: %s\n",temp);
    sleep(1);
}

void writeSellerInfo () {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    FILE*  fl1 = fopen("./txt/SellerInfo.txt","w");
    fprintf(fl1, "seller: %s", temp);
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
    printf("billing: %s\n",temp);
    sleep(1);
}

void writeCustomerOrderInfo() {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    FILE*  fl1 = fopen("./txt/CustomerOrder.txt","w");
    fprintf(fl1, "%s", temp);
    fclose(fl1);
    printf("order: %s\n",temp);
    sleep(1);
}

void writeBuyerInfo() {
    char temp[30] = {0};
    sprintf(temp,"%d", rand());
    FILE*  fl1 = fopen("./txt/CustomerInfo.txt","w");
    fprintf(fl1, "%s", temp);
    fclose(fl1);
    printf("buyer: %s\n",temp);
    sleep(1);
}

void readProductInfo () {
    int x;
    FILE* fl1 = fopen("./txt/ProductInfo.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("product: %d\n", x);
    //sleep(1);
}

void readSellerInfo () {
    int x;
    FILE* fl1 = fopen("./txt/SellerInfo.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("seller: %d\n", x);
    //sleep(1);
}

void readBillingInfo () {
    int x;
    FILE* fl1 = fopen("./txt/BillingInfo.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("billing: %d\n", x);
    //sleep(1);
}

void readCustomerOrderInfo() {
    int x;
    FILE* fl1 = fopen("./txt/CustomerOrder.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("order: %d\n", x);
    //sleep(1);
}

void readBuyerInfo() {
    int x;
    FILE* fl1 = fopen("./txt/CustomerInfo.txt","r");
    fscanf(fl1, "%d", &x);
    fclose(fl1);
    printf("buyer: %d\n", x);
    //sleep(1);
}

#pragma clang diagnostic pop
#pragma clang diagnostic pop