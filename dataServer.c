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


#define DATA_SERVER_PORT_NUM 8080
#define CLIENT_BACKLOG 25
#define MSG_BUFFER_SIZE 10240

///Jacobs Functions  DELETE LATER
int readDatabase(char* personType, char* table, char* personID, char* result){
    char buffer;
    char row[100];
    int position = 0;
    int numCommas = 0;
    char ID[4];
    int checker = 0;
    //opens the file buffer
    FILE* fl1 = fopen(table, "r");
    buffer = fgetc(fl1);
    //loops through every char in the file until the end
    while(buffer != EOF){
        //a weird if statement but it flags checker saying the ID is the same as the target ID
        if(position == 4 && strcmp(personID,row) == 0){
            checker= 1;
        }
        //checks to see if its the end of the full row entry
        if(buffer == '\n'){
            //adds the entry into result if checker is 1
            if(checker ==1){
                strcat(result,row);
            }
            //resets all of the variables for the next row entry
            memset(row,0,strlen(row));
            memset(ID,0,strlen(ID));
            position = -1;
            numCommas = 0;
            checker = 0;
        }
            //builds the row entry
        else{
            row[position] = buffer;
            //printf("%s\n",row);
        }
        position++;
        buffer = fgetc(fl1);
    }
    fclose(fl1);
}

int updateDatabase(char personID[4]){
    char buffer;
    char row[100];
    char helper[100];
    char reinput[1000];
    int position = 0;
    int numCommas = 0;
    char ID[4];
    int checker = 0;
    //opens the file buffer
    FILE* fl1 = fopen("testing.txt", "r+");
    buffer = fgetc(fl1);
    //loops through every char in the file until the end
    while(buffer != EOF){
        //printf("position: %d\n", position);
        if(buffer == '\n' || position == 0){
            //printf("IN IF\n");
            if(position == 0){
                ID[0] = buffer;
                reinput[0] = buffer;
                buffer = fgetc(fl1);
                ID[1] = buffer;
                //position++;
                reinput[1] = buffer;
                buffer = fgetc(fl1);
                ID[2] = buffer;
                //position++;
                reinput[2] = buffer;
                buffer = fgetc(fl1);
                ID[3] = buffer;
                //position++;
                reinput[3] = buffer;
                position = position + 3;
            }
            else{
                reinput[position] = '\n';

                buffer = fgetc(fl1);
                ID[0] = buffer;
                position++;
                reinput[position] = buffer;

                buffer = fgetc(fl1);
                ID[1] = buffer;
                position++;
                reinput[position] = buffer;

                buffer = fgetc(fl1);
                ID[2] = buffer;
                position++;
                reinput[position] = buffer;

                buffer = fgetc(fl1);
                ID[3] = buffer;
                position++;
                reinput[position] = buffer;
                //position = position + 5;
            }
            printf("\nID:%s\n",ID);
            //strcat(reinput,ID);
            printf("%s\n", reinput);
            if(strcmp(ID,personID) == 0){
                printf("Insert new name: ");
                memset(row, 0, strlen(row));
                //scanf("%s\n", row);
                fgets(row, 100, stdin);
                strcat(reinput,",");
                strncpy(row,row,strlen(row) -2);
                printf("\nROW:%s\n",row);
                strcat(reinput,row);
                reinput[strlen(reinput)-1] = 0;

                printf("Insert new Phone Number in form of (xxx)xxx-xxxx: ");
                memset(row, 0, strlen(row));
                //scanf("%s\n", row);
                fgets(row, 100, stdin);
                strcat(reinput,",");
                //strncpy(row,row,strlen(row) -2);
                printf("\nROW:%s\n",row);
                strcat(reinput,row);
                reinput[strlen(reinput)-1] = 0;

                printf("Insert new Address: ");
                memset(row, 0, strlen(row));
                //scanf("%s\n", row);
                fgets(row, 100, stdin);
                strcat(reinput,",");
                strncpy(row,row,strlen(row) -2);
                printf("\nROW:%s\n",row);
                strcat(reinput,row);

                strcat(reinput,"\n");
                position = strlen(reinput)-1;
                buffer = fgetc(fl1);

                while(buffer != '\n'){
                    //printf("\nCHAR NEXT%c\n", buffer);
                    buffer = fgetc(fl1);
                }
                memset(ID, 0, 4);
            }
        }
        else{
            printf("%c",buffer);
            reinput[position] = buffer;
            position++;
        }
        buffer = fgetc(fl1);
    }
    //fprintf(fl1, "%s", reinput);
    printf("\n\n\n%s", reinput);
    fclose(fl1);
}






/*---Function Declarations---*/
void createSocket(int *sock);

void bindSocket(struct sockaddr_in *sAddress, const int *sock);

void listenOnSocket(const int* sock, int backLogCount);

bool acceptConnection(int* clientSock, const int* hSock, struct sockaddr_in* client);

void readSocket(const int* sock, char* buffer);

void writeSocket(const int* sock, char* buffer);

bool validateID(char* buffer, char* dataBase, int* clientSock);

void generateUID(char (*arr)[], char* dataBase);

void extractCommand(char (*buffer)[], char (*command)[]);

void newClientData(char (*buffer)[]);

void viewProducts(char (*buffer)[], int* clientSock);



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


    /*----Forking Clients----*/
    while (1) {
        if (acceptConnection(&clientSock, &hSocket, &client) == true) { //Accept incoming client connection
            pid = fork();                                               //Fork client process
            if(pid == 0) {                                              //code for child(client) process
                close(hSocket);



                //close hSocket, child uses clientSock
                /*---Establish if client is a Buyer or a Seller---*//*
                bzero(buffer, MSG_BUFFER_SIZE);
                readSocket(&clientSock, buffer);
                if(strstr(buffer, "[BUYER]") != NULL) {
                    isBuyer = true;
                } else if (strstr(buffer, "[SELLER]") != NULL) {
                    isBuyer = false;
                }
                bzero(buffer, MSG_BUFFER_SIZE);
                writeSocket(&clientSock, "[CONFIRMATION]");*/

                /*---Set clientID ---*//*
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
                }*/

                /*---Main Logic Loop: Driven by Client Commands ---*/
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
                while(1) {
                    bzero(buffer, MSG_BUFFER_SIZE);
                    readSocket(&clientSock, buffer);
                    extractCommand(&buffer, &command);

                    /*1.store new client data  TCP COMMAND: [NEW_CLIENT]*/
                    //REQUIRED BUFFER STRING: "[NEW_CLIENT],[BUYER],FirstName, LastName, phoneNumber, streetAddress, City, State, zipcode,"  <--notice buffer should end with ','
                    //OR REQUIRED BUFFER STRING: "[NEW_CLIENT],[SELLER],FirstName, LastName, phoneNumber, streetAddress, City, State, zipcode,"
                    if(strstr(command, "[NEW_CLIENT]") != NULL) {

                        printf("New Client!\n");            //for testing
                        newClientData(&buffer);              //send modified buffer to extract contents and write to ConsumerInfo or SellerInfo.txt
                        writeSocket(&clientSock, "[CONFIRMATION]"); //Send confirmation to clientServer
                        bzero(buffer, MSG_BUFFER_SIZE);
                        bzero(command, 25);

                    /*2.Validate an ID in a database  TCP COMMAND: [VALIDATE_ID]*/
                    } else if(strstr(command, "[VALIDATE_ID]") != NULL) {
                        printf("Validating an ID!\n"); //for testing
                        //validateID(&buffer);

                        /*
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
                         */

                    /*3. View Available Products TCP COMMAND: [VIEW_PRODUCTS]*/
                    //REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[BUYER],uuid,"
                    //OR REQUIRED BUFFER STRING: "[VIEW_PRODUCTS],[SELLER],uuid,"
                    } else if(strstr(command, "[VIEW_PRODUCTS]") != NULL) {
                        printf("View products!\n"); //for testing

                        viewProducts(&buffer, &clientSock);



                    /*4. BuyerOPTION 1-3 Complete Order TCP COMMAND: [COMPLETE_ORDER]*/
                    } else if(strstr(command, "[COMPLETE_ORDER]") != NULL) {
                        printf("Completing Order!\n"); //for testing

                        /*READ ProductInfo.txt validate given ProductID's AND Quantities
                        //validateID(ProductID, "ProductInfo.txt")
                        //validateQuantity(int ProductID, int quantity)

                        IF OK --> WRITE CustomerOrder.txt AND WRITE BillingInfo.txt
                            SEND Order Conformation
                        ELSE Invalid
                            SEND Order Invalid*/


                    /*5. BuyerOPTION 2 View Orders TCP COMMAND: [VIEW_BUYER_ORDERS]*/
                    } else if(strstr(command, "[VIEW_BUYER_ORDERS]") != NULL) {
                        printf("View Buyer Order!\n"); //for testing

                        /*READ BillingInfo.txt
                        SEND OrderID, Total Order Price*/


                    /*6. BuyerOPTION 3 Modify Order TCP COMMAND: [MOD_ORDER]*/
                    } else if(strstr(command, "[MOD_ORDER]") != NULL) {
                        printf("Modifying an Order!\n"); //for testing

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
                        printf("Buyer Viewing Billing Info!\n"); //for testing

                       /* READ CustomerInfo.txt
                        SEND Name, Number, Address*/


                    /*8. BuyerOPTION/SellerOPTION 5 Edit Info TCP COMMAND: [EDIT_INFO]*/
                    } else if(strstr(command, "[EDIT_INFO]") != NULL) {
                        printf("Editing Info!\n"); //for testing

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
                        printf("Seller adding a new product!\n"); //for testing
                       /* WRITE ProductInfo.txt
                        new ProductID, productName, SellerID, Quantity, Price/Unit
                        SEND Confirmation*/


                    /*10. SellerOption 3 Delete Product TCP COMMAND: [DELETE_PROD]*/
                    } else if(strstr(command, "[DELETE_PROD]") != NULL) {
                        printf("Seller deleting a product!\n"); //for testing
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


bool validateID(char* buffer, char* dataBase, int* clientSock) {

}

void generateUID(char (*arr)[], char* dataBase) {

    srand(time(NULL));
    int idNum = (rand() % 9000) + 1000;
    sprintf(*arr, "%d", idNum);
    //printf("%s\n", dataBase);

    //check if ID is in dataBase, if not accept, else regenerate

}


void extractCommand(char (*buffer)[], char (*command)[]) {
    char* ptr;
    ptr = strtok(*buffer,",");

    if(ptr) {
        sprintf(*command, "%s", ptr);
    }
    ptr = strtok(NULL, "");

    if(ptr)
        sprintf(*buffer, "%s\n", ptr);
}

void newClientData(char (*buffer)[]) {
    char uuid[5] = {0};
    char clientType[25] = {0};
    char firstName[25] = {0};
    char lastName[25] = {0};
    char phoneNumber[25] = {0};
    char streetAddress[25] = {0};
    char city[25] = {0};
    char state[25] = {0};
    char zipCode[25] = {0};
    char* ptr;

    //extract clientType
    ptr = strtok(*buffer,",");
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
        generateUID(&uuid, "CustomerInfo.txt");
       //updateDatabase(uuid, "CustomerInfo.txt");
    } else {
        generateUID(&uuid, "SellerInfo.txt");
        //updateDatabase(uuid, "SellerInfo.txt");
    }
}

void viewProducts(char (*buffer)[], int* clientSock) {
    char uuid[5] = {0};
    char clientType[25] = {0};
    char* ptr;

    //extract clientType
    ptr = strtok(*buffer,",");
    sprintf(clientType, "%s", ptr);

    //extract uuID
    ptr = strtok(*buffer,",");
    sprintf(uuid, "%s", ptr);

    if (strstr(clientType, "[BUYER]") != NULL) { //IF Buyer
        /*READ ProductInfo.txt
        SEND ALL: ProductIDs, Product Names, Quantitys, Price/Units*/
    } else {                                            //IF Seller
        /*READ ProductInfo.txt
        SEND FILTERED to Seller's ID:  ProductIDs, Product Names, Quantitys, Price/Units*/
    }
}