/*
    Group A
    Author: Jeremiah Pete
    Email: jeremiah.pete@okstate.edu

    Program Description: This file handles the structuring of the client side of the program,
    both the buyer and seller

    To run file use command "gcc -o clientServer clientServer.c -lpthread" */

#include "client.h"

/*  function to connect the client to the server
    params: none
    returns: int (the client value) */
int clientConnect(struct sockaddr_in *serverAddress, int clientSocket) {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0); // create socket

    if (clientSocket < 0) {
        printf("\nError creating socket\n");
        return -1;
    }

    memset(&serverAddress, 0, sizeof(serverAddress)); // clear memory

    // set values for server struct port
    serverAddress->sin_family = AF_INET;
    serverAddress->sin_port = htons(PORT);
    serverAddress->sin_addr.s_addr = inet_addr("127.0.0.1");

    // establish connection to server
    int connection = connect(clientSocket, (struct sockaddr *)&(*serverAddress), sizeof(*serverAddress));

    // ensure the connection is valid
    if (connection < 0) {
        printf("\nBinding connection failed! Please make sure server is running\n");
        exit(0);
    }

    printf("\nSuccessfully connected to server!\n");
}

/*  function to read/write between client and server
    params: int clientSocket
    returns: void */
void clientComm(int clientSocket, char *message) {
    // declare variables to be used
    char buff[25];
    int size;

    while (1) {
        memset(buff, 0, sizeof(buff)); // clear buffer
        size = 0;

        while((buff[size++] = getchar()) != '\n')
            ; // do nothing
        write(clientSocket, buff, sizeof(buff));
        memset(buff, 0, sizeof(buff));
        read(clientSocket, buff, sizeof(buff));
        printf("From server: %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("\nExiting...\n");
            break;
        }
    }
}

/*  function to send data from client to server
    params: int socket, char* command
    returns: void */
void clientSend(int *socketVal, char *buff) {
    if (write(*socketVal, buff, 10240) == -1) {
        printf("\nCould not write to socket\n\n");
        exit(0);
    }
}

/*  function to receive data from server
    params: int socket, char* receive
    returns: void */
void clientReceive(int *socketVal, char *buff) {
    if (read(*socketVal, buff, 10240) == -1) {
        printf("\nCould not read from socket\n\n");
        exit(1);
    }
}

/*  function to display and handle the initial menu of the program
    params: int clientSock
    returns: void */
void initialMenu(int clientSock) {
    int selection;

    printf("\nPlease select an option below - \n\n");
    // display options to user
    while(1) {
        printf("1. Buyer Menu\n");
        printf("2. Seller Menu\n");
        printf("3. Exit Program\n");
        printf("\nSelection: ");

        // retrieve selecttion
        scanf("%d", &selection);

        // use switch case to determine which menu to show
        switch (selection) {
            case 1: // go to buyerLogin()
                buyerLogin(clientSock);
            case 2: // go to sellerLogin()
                sellerLogin(clientSock);
            case 3: // exit program
                printf("\nProgram Terminated\n");
                exit(0);
            default:
                printf("\nPlease select a valid choice!\n\n");
        }
    }
}

/*  function to handle when the user chooses to login as a buyer
    params: none
    returns: void */
void buyerLogin(int clientSock) {
    int select;
    int type = 0; // for register option

    //system("clear"); // clear the screen for better tracking
    printf("\n\nBuyer Login:\n\nPlease select an option -\n\n");

    // display options to user
    while(1) {
        printf("1. New Customer Registration\n");
        printf("2. Login with Customer ID\n");
        printf("3. Back to Main Menu\n");
        printf("4. Exit Program\n");
        printf("\nSelection: ");

        scanf("%d", &select);

        // use switch case to make decision
        switch (select) {
            case 1: // new customer registration
                //system("clear");
                userRegister(type, clientSock);
            case 2: // login with customer ID
                IDLogin(type, clientSock);
            case 3: // go back to main menu
                //system("clear");
                initialMenu(clientSock);
            case 4:
                exit(0);
            default:
                printf("\nPlease select a valid choice!\n\n");
        }
    }
}

/*  function to handle when the user chooses to login as a seller
    params: none
    returns: void */
void sellerLogin(int clientSock) {
    int select;
    int type = 1; // for register option

    //system("clear");
    printf("\n\nSeller Login:\n\nPlease select an option -\n\n");

    // display options to user
    while(1) {
        printf("1. New Seller Registration\n");
        printf("2. Login with Seller ID\n");
        printf("3. Back to Main Menu\n");
        printf("4. Exit Program\n");
        printf("\nSelection: ");

        scanf("%d", &select); // get input

        // use switch case to make decision
        switch (select) {
            case 1: // new seller registration
                //system("clear");
                userRegister(type, clientSock);
            case 2: // login with sellerID
                IDLogin(type, clientSock);
            case 3: // go back to the initial menu
                //system("clear");
                initialMenu(clientSock);
            case 4: // exit program
                exit(0);
            default:
                printf("\nPlease select a valid choice!\n\n");
        }
    }
}


/*  function to register the user (as buyer or seller)
    params: int type (to distinguish between buyer and seller)
    returns: void */
void userRegister(int type, int clientSock) {
    // declare variables to store info needed for registering users
    char firstName[50] = {0};
    char lastName[50] = {0};
    char num[50] = {0};
    char address[100] = {0};
    char buffer[10240] = {0};

    if (type == 0) { // if it's a buyer registering
        // get name info
        printf("Please Enter First Name: ");
        scanf(" %[^\n]", firstName);

        printf("\nPlease Enter Last Name: ");
        scanf(" %[^\n]", lastName);

        printf("\nHello, %s %s!", firstName, lastName); // return full name to make sure input is correct

        // get number info
        printf("\n\nPlease Enter Contact Number: ");
        scanf(" %[^\n]", num);
        printf("\nNumber Entered: %s", num); // return number to make sure input is correct

        // get address info
        printf("\n\nPlease Enter Address (Format: Street Name,City,State,Zip Code ***no spaces between commas):");
        printf("\nExample: 1234 Street St.,Edmond,OK,73003\n\nAddress: ");
        scanf(" %[^\n]", address);
        printf("\nAddress Entered: %s", address); // return address to make sure input is correct

        /** TODO: Write info to client server **/
        strcat(buffer, "[NEW_CLIENT],[BUYER],");
        strcat(buffer, firstName);
        strcat(buffer, lastName);
        strcat(buffer, num);
        strcat(buffer, address);
        strcat(buffer, ",");

        clientSend(&clientSock, buffer);
        bzero(buffer, 10240);
        clientReceive(&clientSock, buffer);

        printf("\nServer Message: %s\n", buffer);

        buyerMenu(clientSock); // go to buyer menu
    }
    else { // if it's a seller registering
        //file = fopen("SellerInfoTest.txt", "a"); // open SellerInfo.txt for appending ID, name, num, and address
        /*
        if (file == NULL) { // in case file is inaccessible
            printf("\nError accessing file SellerInfo.txt!\n");
            exit(1);
        } */

        // get name info
        printf("Please Enter First Name: ");
        scanf(" %[^\n]", firstName);

        printf("\nPlease Enter Last Name: ");
        scanf(" %[^\n]", lastName);

        printf("\nHello, %s %s!", firstName, lastName); // return full name

        // get number info
        printf("\n\nPlease Enter Contact Number: ");
        scanf(" %[^\n]", num);
        printf("\nNumber Entered: %s", num); // return number to make sure input is correct

        // get address info
        printf("\n\nPlease Enter Address (Format: Street Name,City,State,Zip Code ***no spaces between commas):");
        printf("\nExample: 1234 Street St.,Edmond,OK,73003\n\nAddress: ");
        scanf(" %[^\n]", address);
        printf("\nAddress Entered: %s", address); // return address to make sure input is correct

        /** TODO: Write info to client server **/
        //fprintf(file, "%s,%s,%s,%s,\n", firstName, lastName, num, address);

        sellerMenu(clientSock); // go to seller menu
    }
}

/*  function to handle the login of the user (both buyer and seller)
    params: int type (to distinct between buyer and seller)
    returns: void */
void IDLogin(int type, int clientSock) {
    /** TODO: Perform a check to the database that determines if the credentials are valid or not **/
}

/*  function to handle the buyer menu model once successfully logged in
    params: none
    returns: void */
void buyerMenu(int clientSock) {
    int select;
    int type = 0; // to distinguish from seller

    //system("clear");
    printf("\n\nBuyer Menu:\n\nPlease select an option -\n\n");

    // display options to user
    while(1) {
        printf("1. Make an Order\n");
        printf("2. View Orders\n");
        printf("3. Modify Order\n");
        printf("4. View Billing Info.\n");
        printf("5. Edit Info.\n");
        printf("6. Back to Main Menu\n");
        printf("7. Exit Program\n");
        printf("\nSelection: ");

        scanf("%d", &select); // get input

        // use switch case for handling
        switch (select) {
            case 1: // make an order
                makeOrder(clientSock);
            case 2: // view orders
                viewOrder();
            case 3: // modify order
                modifyOrder();
            case 4: // view billing info
                viewBill();
            case 5: // edit info
                editInfo(type, clientSock);
            case 6: // back to main menu
                initialMenu(clientSock);
            case 7: // exit case
                exit(0);
            default:
                printf("Please select a valid choice!\n\n");
        }
    }
}

/*  function to let buyer make an order
    params: none
    returns: void */
void makeOrder(int clientSock) {
    int select;

    // display options to user
    while(1) {
        printf("1. View Available Products\n");
        printf("2. Add Product to Order\n");
        printf("3. Complete Order\n");
        printf("4. Cancel/Back to Buyer Menu\n");
        printf("5. Terminate Program\n\n");
        printf("Selection: ");

        scanf("%d", &select); // get selection

        // handle selection
        switch (select) {
            case 1: // view available products
                viewProducts();
            case 2: // add to order
                addProduct();
            case 3: // complete order
                completeOrder();
            case 4: // back to buyer menu
                buyerMenu(clientSock);
            case 5: // exit program
                printf("\nProgram Terminated\n");
                exit(0);
            default:
                printf("\n\nPlease select a proper choice!\n\n");
        }
    }
}

/*  funtion to let buyer view orders, behaves similar to a thread and uses semaphores
    params: none
    returns: void */
void viewOrder() {
    //sem_wait(&lock); // wait
    printf("\nCritical Section Entered...\n"); // just a test
    // critical section will validate ID
    /*** TODO: lock BillingInfo.txt and load necessary data into buffer for display ***/
    printf("\nExiting Critical Section\n");
    //sem_post(&lock); // signal to exit crit section
}

/*  function to let buyer modify order
    params: none
    returns: void */
void modifyOrder() {
    /*** TODO: lock BillingInfo.txt and check if the ID is valid ***/
}

/*  function to let buyer view billing info
    params: none
    returns: void */
void viewBill() {
    /*** TODO: lock CustomerInfo.txt and load data into buffer ***/
}

/*  function to edit stored info
    params: none
    returns: void */
void editInfo(int type, int clientSock) {
    /*** TODO: let user edit their info as necessary, maybe use params to help transfer data ***/
    // initialize variables to be used
    char newName[100];
    char newNum[50];
    char newAddress[100];
    int select;

    printf("1. Edit Name\n");
    printf("2. Edit Number\n");
    printf("3. Edit Address\n");
    printf("4. Go Back\n\n");
    printf("Selection: ");
    // get input from user
    scanf("%d", &select);

    switch (select) {
        case 1: // edit name
            printf("\nEnter New Name: ");
            scanf(" %[^\n]", newName);
            printf("New name entered, hello %s!", newName);
        case 2: // edit number
            printf("\nEnter New Number: ");
            scanf(" %[^\n]", newNum);
            printf("New number entered, %s", newNum);
        case 3: // edit address
            printf("\nEnter New Address: ");
            scanf(" %[^\n]", newAddress);
            printf("New address entered, %s", newAddress);
        case 4: // return to whichever menu came from
            if (type == 0)
                buyerMenu(clientSock);
            else
                sellerMenu(clientSock);
    }
}

/*  function to let buyer complete their order
    params: none
    returns: void */
void completeOrder() {
    /*** TODO: Lock ProductInfo.txt and validate ID and quantity, perform updates ***/
}

/*  function to let seller view products
    params: none
    returns: void */
void viewProducts() {
    /*** TODO: lock ProductInfo.txt, load data into buffer, return to menu ***/
    // data = (ProductID, ProductName, Quantity, Price/Unit)
}

/*  function for seller to add product to list
    params: none
    returns: void */
void addProduct() {
    // initialize storage values to be read
    char product[100];
    char quantity[10];
    char price[10];

    // get product name
    printf("Enter Product Name: ");
    scanf(" %[^\n]", product);

    // get quantity amount
    printf("Enter Quantity: ");
    scanf(" %[^\n]", quantity);

    // get price amount
    printf("Enter Price/Unit: ");
    scanf(" %[^\n]", price);
}

/*  function to handle the seller menu model once successfully logged in
    params: none
    returns: void */
void sellerMenu(int clientSock) {
    int select;
    int type = 1; // to distinguish from buyer

    //system("clear");
    printf("\n\nSeller Menu:\n\nPlease select an option -\n\n");

    // display options to user
    while(1) {
        printf("1. View Offered Products\n");
        printf("2. Add New Product\n");
        printf("3. Delete Product\n");
        printf("4. Modify a Product Quantity\n");
        printf("5. Modify a Product's Price\n");
        printf("6. View Product Orders");
        printf("7. Edit Seller Info.\n");
        printf("8. Return to Main Menu\n");
        printf("9. Exit Program\n");
        printf("\nSelection: ");

        scanf("%d", &select); // get input

        // use switch case for handling
        switch (select) {
            case 1: // view offered products
                viewProducts();
            case 2: // add new product
                addNewProduct();
            case 3: // delete product
                deleteProduct();
            case 4: // modify quantity
                modifyQuantity();
            case 5: //  modify price
                modifyPrice();
            case 6: // view product orders
                viewOrder();
            case 7: // edit seller info
                editInfo(type, clientSock);
            case 8: // return to main menu
                initialMenu(clientSock);
            case 9: // exit case
                printf("Program Terminated\n");
                exit(0);
            default:
                printf("Please select a valid choice!\n\n");
        }
    }
}

/*  function for seller to add new product to list
    params: none
    returns: void */
void addNewProduct() {
    // initialize variables
    char name[100];
    char quantity[100];
    char price[10];

    // get product name
    printf("Enter Product Name: ");
    scanf(" %[^\n]", name);

    // get product quantity
    printf("Enter Quantity Available: ");
    scanf(" %[^\n]", quantity);

    // get product price
    printf("Enter Product Price: ");
    scanf(" %[^\n]", price);

    /*** TODO: Lock ProductInfo.txt, load info into buffer, unlock ProductInfo.txt and send the data through ***/
}

/*  function to delete product from database
    params: none
    returns: void */
void deleteProduct() {
    char ID;

    printf("Enter ProductID to Remove: ");
    scanf("%c", &ID);

    /*** TODO: Update CustomerOrder and get total price in order to modify value ***/
}

/*  function to modify the quantity available of an item
    params: none
    returns: void */
void modifyQuantity() {
    char ID;
    int quantity;

    printf("Enter ProductID to Adjust: ");
    scanf("%c", &ID);

    /*** TODO: Locate Product from database ***/

    printf("Enter quantity to set it to: ");
    scanf("%d", &quantity);

    /*** TODO: Lock ProductInfo.txt, validate ID, etc ***/
}

/*  function to modify the price of an item
    params: the item to be selected
    returns: the new price of the item (void) */
void modifyPrice() {
    char ID;
    int price;

    printf("Enter ProductID to Adjust: ");
    scanf("%c", &ID);

    /*** TODO: Locate Product from database ***/

    printf("Enter price to set it to: ");
    scanf("%d", &price);

    /*** TODO: Lock ProductInfo.txt, validate ID, etc ***/
}

// main function to test functions
int main() {
    // I've basically put the clientConnect() function here to test the issues I've been having
    char buffer[1024];
    char message[25];
    //int clientSock;
    struct sockaddr_in server, client;
    // create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("\nError creating socket\n");
        return -1;
    }

    memset(&server, 0, sizeof(server)); // clear memory

    // set values for server struct port
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // establish connection to server
    int connection = connect(clientSocket, (struct sockaddr *)&server, sizeof(struct sockaddr_in));

    // ensure the connection is valid
    if (connection < 0) {
        printf("\nBinding connection failed! Please make sure server is running\n");
        exit(0);
    }

    printf("\nSuccessfully connected to server!\n");
    //clientConnect(&server, clientSock);

    // begin the program for client
    initialMenu(clientSocket);

    // close the socket when finished
    close(clientSocket);
}