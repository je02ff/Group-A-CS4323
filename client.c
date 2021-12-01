/*  Group A
    Author: Jeremiah Pete
    Email: jeremiah.pete@okstate.edu
    Program Description: This file handles the structuring of the client side of the program,
    both the buyer and seller   
    
    To run file use command "gcc -o client client.c -lpthread" */

#include "client.h"

// create global buffer variables
char buffer[BUFFERSIZE] = {0}; 
// declare variables to store info needed for registering users
char buyerID = {0}, sellerID, productID, orderID;
char firstName[50] = {0};
char lastName[50] = {0};
char num[50] = {0};
char address[100] = {0};
char city[50] = {0};
char state[50] = {0};
char zipCode[10] = {0};
char quantity[100] = {0};
char price[100] = {0};

/*  function to send data from client to server
    params: int socket, char* command
    returns: void */
void clientSend(int *socketVal, char *buff) {
    if (write(*socketVal, buff, BUFFERSIZE) == -1) {
        printf("\nCould not write to socket\n\n");
        exit(0);
    }
}

/*  function to receive data from server
    params: int socket, char* receive
    returns: void */
void clientReceive(int *socketVal, char *buff) {
    if (read(*socketVal, buff, BUFFERSIZE) == -1) {
        printf("\nCould not read from socket\n\n");
        exit(1);
    }
}

/*  function to display and handle the initial menu of the program
    params: int clientSock
    returns: void */
void initialMenu(int clientSock) {
    int selection;
    memset(buffer, 0, BUFFERSIZE);
while(1){
    printf("\nPlease select an option below - \n\n");
    // display options to user
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
            break;
        case 2: // go to sellerLogin()
            sellerLogin(clientSock);
            break;
        case 3: // exit program
            printf("\nProgram Terminated\n");
            exit(0);
        default:
            printf("\nPlease select a valid choice!\n\n");
            initialMenu(clientSock);
            break;
    }
    }
}

/*  function to handle when the user chooses to login as a buyer
    params: int clientSock
    returns: void */
void buyerLogin(int clientSock) {
    int select;
    int type = 0; // for register option
while(1){
    printf("\n\nBuyer Login:\n\nPlease select an option -\n\n");
    
    // display options to user
    printf("1. New Customer Registration\n");
    printf("2. Login with Customer ID\n");
    printf("3. Back to Main Menu\n");
    printf("4. Exit Program\n");
    printf("\nSelection: ");

    scanf("%d", &select);

    // use switch case to make decision
    
    switch (select) {
        case 1: // new customer registration
            userRegister(type, clientSock);
            break;
        case 2: // login with customer ID
            IDLogin(type, clientSock);
            break;
        case 3: // go back to main menu
            initialMenu(clientSock);
            break;
        case 4:
            exit(0);
        default:
            printf("\nPlease select a valid choice!\n\n");
            buyerLogin(clientSock);
            break;
    }
    }
}

/*  function to handle when the user chooses to login as a seller
    params: int clientSock
    returns: void */
void sellerLogin(int clientSock) {
    int select;
    int type = 1; // for register option
while(1){
    printf("\n\nSeller Login:\n\nPlease select an option -\n\n");

    // display options to user
    printf("1. New Seller Registration\n");
    printf("2. Login with Seller ID\n");
    printf("3. Back to Main Menu\n");
    printf("4. Exit Program\n");
    printf("\nSelection: ");

    scanf("%d", &select); // get input

    // use switch case to make decision
    
    switch (select) {
    case 1: // new seller registration
        userRegister(type, clientSock);
        break;
    case 2: // login with sellerID
        IDLogin(type, clientSock);
        break;
    case 3: // go back to the initial menu
        initialMenu(clientSock);
        break;
    case 4: // exit program
        exit(0);
    default:
        printf("\nPlease select a valid choice!\n\n");
        sellerMenu(clientSock);
        break;
    }
    }
}

/*  function to register the user (as buyer or seller)
    params: int type, int clientSock
    returns: void */
void userRegister(int type, int clientSock) {
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
        printf("\n\nPlease Enter Street Address: ");
        scanf(" %[^\n]", address);
        
        // get city
        printf("\nPlease Enter City: ");
        scanf(" %[^\n]", city);

        // get state
        printf("\nPlease Enter State: ");
        scanf(" %[^\n]", state);

        // get zipcode
        printf("\nPlease Enter Zip Code: ");
        scanf(" %[^\n]", zipCode);

        printf("\n\nAddress Entered: %s,%s,%s,%s\n", address, city, state, zipCode);

        /** Write info to client server **/
        strcat(buffer, "[NEW_CLIENT],[BUYER],");
        strcat(buffer, firstName);
        strcat(buffer, ",");
        strcat(buffer, lastName);
        strcat(buffer, ",");
        strcat(buffer, num);
        strcat(buffer, ",");
        strcat(buffer, address);
        strcat(buffer, ",");
        strcat(buffer, city);
        strcat(buffer, ",");
        strcat(buffer, state);
        strcat(buffer, ",");
        strcat(buffer, zipCode);
        strcat(buffer, ",");
        
        clientSend(&clientSock, buffer);
        memset(buffer, 0, BUFFERSIZE); // clear buffer
        clientReceive(&clientSock, buffer);

        printf("\nServer Message: %s\n", buffer);
        buyerID = *buffer; // buffer should return newClientID 
        
        buyerMenu(clientSock); // go to buyer menu
    }
    else { // if it's a seller registering
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
        printf("\n\nPlease Enter Street Address: ");
        scanf(" %[^\n]", address);
        
        // get city
        printf("\nPlease Enter City: ");
        scanf(" %[^\n]", city);

        // get state
        printf("\nPlease Enter State: ");
        scanf(" %[^\n]", state);

        // get zipcode
        printf("\nPlease Enter Zip Code: ");
        scanf(" %[^\n]", zipCode);

        printf("\n\nAddress Entered: %s,%s,%s,%s\n", address, city, state, zipCode);

        /** Write info to client server **/
        strcat(buffer, "[NEW_CLIENT],[BUYER],");
        strcat(buffer, firstName);
        strcat(buffer, ",");
        strcat(buffer, lastName);
        strcat(buffer, ",");
        strcat(buffer, num);
        strcat(buffer, ",");
        strcat(buffer, address);
        strcat(buffer, ",");
        strcat(buffer, city);
        strcat(buffer, ",");
        strcat(buffer, state);
        strcat(buffer, ",");
        strcat(buffer, zipCode);
        strcat(buffer, ",");
        
        clientSend(&clientSock, buffer);
        memset(buffer, 0, BUFFERSIZE); // clear buffer
        clientReceive(&clientSock, buffer);

        printf("\nServer Message: %s\n", buffer);
        sellerID = *buffer; // buffer should return newClientID 

        sellerMenu(clientSock); // go to seller menu
    }
}

/*  function to handle the login of the user (both buyer and seller)
    params: int type (to distinct between buyer and seller), int clientSock
    returns: void */
void IDLogin(int type, int clientSock) {
    char ID[50] = {0}; // to retrieve ID input

    //memset(buffer, 0, BUFFERSIZE);
    printf("Please Enter ID to Login: ");
    scanf(" %[^\n]", ID); // retrieve ID value

    /** Perform a check to the database that determines if the credentials are valid or not **/
    if (type == 0) { // if it's a buyer logging in
        strcat(buffer, "[VALIDATE_ID],");
        strcat(buffer, ID);
        strcat(buffer,",[BUYER],");

        clientSend(&clientSock, buffer); // send message to server
        memset(buffer, 0, BUFFERSIZE);
        clientReceive(&clientSock, buffer); // receive message from server

        printf("\nServer Message: %s\n\n", buffer); // [CONFIRMATION] OR [INVALID]
        
        // determine actions if login is successful or not
        if (strcmp(buffer,"[CONFIRMATION]") == 0) {
            buyerMenu(clientSock);
        }
        else {
            printf("\nLogin invalid!\n");
            initialMenu(clientSock); // return to the initial menu if login unsuccessful
        }
    }
    else { // if it's a seller logging in 
        strcat(buffer, "[VALIDATE_ID],");
        strcat(buffer, ID);
        strcat(buffer,",[SELLER],");

        clientSend(&clientSock, buffer); // send message to server
        memset(buffer, 0, BUFFERSIZE);
        clientReceive(&clientSock, buffer); // receive message from server

        printf("\nServer Message: %s\n", buffer); // [CONFIRMATION] OR [INVALID]

        // determine actions if login is successful or not
        if (strcmp(buffer,"[CONFIRMATION]") == 0)
            sellerMenu(clientSock);
        else 
            initialMenu(clientSock); // return to the initial menu if login unsuccessful
    }
}

/*  function to handle the buyer menu model once successfully logged in
    params: int clientSock
    returns: void */
void buyerMenu(int clientSock) {
    int select;
    int type = 0; // to distinguish from seller
while(1){
    printf("\n\nBuyer Menu:\n\nPlease select an option -\n\n");

    // display options to user
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
    
    switch (select)
    {
    case 1: // make an order
        makeOrder(clientSock);
        break;
    case 2: // view orders
        viewOrder(clientSock);
        break;
    case 3: // modify order
        modifyOrder(clientSock);
        break;
    case 4: // view billing info
        viewBill(clientSock);
        break;
    case 5: // edit info
        editInfo(type, clientSock);
        break;
    case 6: // back to main menu
        initialMenu(clientSock);
        break;
    case 7: // exit case
        exit(0);
    default:
        printf("Please select a valid choice!\n\n");
        buyerMenu(clientSock);
        break;
    }
    }
}

/*  function to handle the seller menu model once successfully logged in
    params: int clientSock
    returns: void */
void sellerMenu(int clientSock) {
    int select;
    int type = 1; // to distinguish from buyer
while(1){
    printf("\n\nSeller Menu:\n\nPlease select an option -\n\n");

    // display options to user
        printf("1. View Offered Products\n");
        printf("2. Add New Product\n");
        printf("3. Delete Product\n");
        printf("4. Modify a Product Quantity\n");
        printf("5. Modify a Product's Price\n");
        printf("6. View Product Orders\n");
        printf("7. Edit Seller Info.\n");
        printf("8. Return to Main Menu\n");
        printf("9. Exit Program\n");
        printf("\nSelection: ");

        scanf("%d", &select); // get input

        // use switch case for handling
        
        switch (select) {
            case 1: // view offered products
                viewProducts(clientSock, type);
                break;
            case 2: // add new product 
                addNewProduct(clientSock);
                break;
            case 3: // delete product
                deleteProduct(clientSock);
                break;
            case 4: // modify quantity
                modifyQuantity(clientSock);
                break;
            case 5: //  modify price
                modifyPrice(clientSock);
                break;
            case 6: // view product orders
                viewOrder(clientSock);
                break;
            case 7: // edit seller info
                editInfo(type, clientSock);
                break;
            case 8: // return to main menu
                initialMenu(clientSock);
                break;
            case 9: // exit case
                printf("Program Terminated\n");
                exit(0);
            default:
                printf("Please select a valid choice!\n\n");
                sellerMenu(clientSock);
                break;
        }
        }
}

/*  function to let buyer make an order
    params: int clientSock
    returns: void */
void makeOrder(int clientSock) {
    int select;
    int type = 0;
while(1){
    // display options to user
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
                viewProducts(clientSock, type);
                break;
            case 2: // add to order
                addProduct(clientSock);
                break;
            case 3: // complete order
                completeOrder(clientSock);
                break;
            case 4: // back to buyer menu
                buyerMenu(clientSock);
                break;
            case 5: // exit program
                printf("\nProgram Terminated\n");
                exit(0);
            default:
                printf("\n\nPlease select a proper choice!\n\n");
                makeOrder(clientSock);
                break;
        }
        }
}

/*  funtion to let buyer view orders, behaves similar to a thread and uses semaphores
    params: int clientSock
    returns: void */
void viewOrder(int clientSock) {
    memset(buffer, 0, BUFFERSIZE); // clear buffer before writing message
    strcat(buffer, "[VIEW_BUYER_ORDERS],");
    strcat(buffer, &buyerID);
    strcat(buffer,",");

    /** Write info to server **/
    clientSend(&clientSock, buffer);
    memset(buffer, 0, BUFFERSIZE); // clear buffer
    clientReceive(&clientSock, buffer);

    printf("\nServer Message: %s\n", buffer); // SENDS LIST OF: ProductIDs,Product Names,Quantity,PricePerUnit
}

/*  function to let buyer modify order
    params: int clientSock
    returns: void */
void modifyOrder(int clientSock) {
    // retreive the orderID and productID
    printf("\nPlease Enter Order ID: ");
    scanf(" %[^\n]", &orderID);

    printf("\nPlease Enter Product ID: ");
    scanf(" %[^\n]", &productID);

    memset(buffer, 0, BUFFERSIZE);

    // first check if the orderID is valid
    strcat(buffer, "[VALIDATE_ID],");
    strcat(buffer, &orderID);
    strcat(buffer, ",[BILLING],");

    /** Write info to server **/
    clientSend(&clientSock, buffer);
    memset(buffer, 0, BUFFERSIZE); // clear buffer
    clientReceive(&clientSock, buffer);

    printf("\nServer Message (ORDER ID): %s\n", buffer);

    // check if the productID is valid
    memset(buffer, 0, BUFFERSIZE); // clear buffer
    strcat(buffer, "[VALIDATE_ID],");
    strcat(buffer, &productID);
    strcat(buffer, ",[ORDER],");

    /** Write info to server **/
    clientSend(&clientSock, buffer);
    memset(buffer, 0, BUFFERSIZE); // clear buffer
    clientReceive(&clientSock, buffer);

    printf("\nServer Message (PRODUCT ID): %s\n", buffer);

    // check if buyerID is valid
    memset(buffer, 0, BUFFERSIZE); // clear buffer
    strcat(buffer, "[CHECK_BUYER],");
    strcat(buffer, &buyerID);
    strcat(buffer, ",");
    strcat(buffer, &orderID);
    strcat(buffer, ",");

    /** Write info to server **/
    clientSend(&clientSock, buffer);
    memset(buffer, 0, BUFFERSIZE); // clear buffer
    clientReceive(&clientSock, buffer);

    printf("\nServer Message: %s\n", buffer);

    // modify order if the ID is valid
    if (strcmp(buffer,"[CONFIRMATION]") == 0) {
        memset(buffer, 0, BUFFERSIZE);

        strcat(buffer, "[ORDER_DETAILS],");
        strcat(buffer, &productID);
        strcat(buffer, ",");

        /** Write info to server **/
        clientSend(&clientSock, buffer);
        memset(buffer, 0, BUFFERSIZE); // clear buffer
        clientReceive(&clientSock, buffer);

        printf("\nServer Message: %s\n", buffer);

        // send [MOD_ORDER] command
        memset(buffer, 0, BUFFERSIZE);

        strcat(buffer, "[MOD_ORDER],");
        strcat(buffer, &orderID);
        strcat(buffer, ",");
        strcat(buffer, &productID);
        strcat(buffer, ",");

        /** Write info to server **/
        clientSend(&clientSock, buffer);
        memset(buffer, 0, BUFFERSIZE); // clear buffer
        clientReceive(&clientSock, buffer);

        printf("\nServer Message: %s\n", buffer);
    }
}

/*  function to let buyer view billing info 
    params: int clientSock
    returns: void */
void viewBill(int clientSock) {
    memset(buffer, 0, BUFFERSIZE);

    strcat(buffer, "[VIEW_BILLING],");
    strcat(buffer, &buyerID);
    strcat(buffer, ",");

    /** Write info to server **/
    clientSend(&clientSock, buffer);
    memset(buffer, 0, BUFFERSIZE); // clear buffer
    clientReceive(&clientSock, buffer);

    //printf("\nServer Message: %s\n", buffer);
}

/*  function to edit stored info
    params: int type, int clientSock
    returns: void */
void editInfo(int type, int clientSock) {
    int select;
while(1){
    printf("1. Edit Name\n");
    printf("2. Edit Number\n");
    printf("3. Edit Address\n");
    printf("4. Go Back\n\n");
    printf("Selection: ");
    // get input from user
    scanf("%d", &select);

    switch (select) {
        case 1: // edit name
            printf("\nEnter New First Name: ");
            scanf(" %[^\n]", firstName);
            printf("\n\nEnter New Last Name: ");
            scanf(" %[^\n]", lastName);
            printf("New name entered, hello %s %s!", firstName, lastName);

            if (type == 0) { // if it's a buyer editing
                memset(buffer, 0, BUFFERSIZE);   // clear any potential leftover memory in buffer

                strcat(buffer, "[EDIT_INFO],[BUYER],");
                strcat(buffer, &buyerID);
                strcat(buffer, ",[NAME],");
                strcat(buffer, firstName);
                strcat(buffer, ",");
                strcat(buffer, lastName);
                strcat(buffer, ",");

                clientSend(&clientSock, buffer); // send message to server
                memset(buffer, 0, BUFFERSIZE);
                clientReceive(&clientSock, buffer); // receive message from server

                printf("\nServer Message: %s\n", buffer); // display message to client
            }
            else { // if it's a seller editing
                memset(buffer, 0, BUFFERSIZE);   // clear any potential leftover memory in buffer

                strcat(buffer, "[EDIT_INFO],[SELLER],");
                strcat(buffer, &sellerID);
                strcat(buffer, ",[NAME],");
                strcat(buffer, firstName);
                strcat(buffer, ",");
                strcat(buffer, lastName);
                strcat(buffer, ",");

                clientSend(&clientSock, buffer); // send message to server
                memset(buffer, 0, BUFFERSIZE);
                clientReceive(&clientSock, buffer); // receive message from server

                printf("\nServer Message: %s\n", buffer); // display message to client
            }
            break;
        case 2: // edit number
            printf("\nEnter New Number: ");
            scanf(" %[^\n]", num);
            printf("New number entered, %s", num);

            if (type == 0) { // if it's a buyer editing
                memset(buffer, 0, BUFFERSIZE);   // clear any potential leftover memory in buffer

                strcat(buffer, "[EDIT_INFO],[BUYER],");
                strcat(buffer, &buyerID);
                strcat(buffer, ",[NUMBER],");
                strcat(buffer, num);
                strcat(buffer, ",");

                clientSend(&clientSock, buffer); // send message to server
                memset(buffer, 0, BUFFERSIZE);
                clientReceive(&clientSock, buffer); // receive message from server

                printf("\nServer Message: %s\n", buffer); // display message to client
            }
            else { // if it's a seller editing
                memset(buffer, 0, BUFFERSIZE);   // clear any potential leftover memory in buffer

                strcat(buffer, "[EDIT_INFO],[SELLER],");
                strcat(buffer, &sellerID);
                strcat(buffer, ",[NUMBER],");
                strcat(buffer, num);
                strcat(buffer, ",");

                clientSend(&clientSock, buffer); // send message to server
                memset(buffer, 0, BUFFERSIZE);
                clientReceive(&clientSock, buffer); // receive message from server

                printf("\nServer Message: %s\n", buffer); // display message to client
            }
            break;
        case 3: // edit address
            printf("\nEnter New Address: ");
            scanf(" %[^\n]", address);
            printf("New address entered, %s", address);

            if (type == 0) { // if it's a buyer editing
                memset(buffer, 0, BUFFERSIZE);   // clear any potential leftover memory in buffer

                strcat(buffer, "[EDIT_INFO],[BUYER],");
                strcat(buffer, &buyerID);
                strcat(buffer, ",[ADDRESS],");
                strcat(buffer, address);
                strcat(buffer, ",");

                clientSend(&clientSock, buffer); // send message to server
                memset(buffer, 0, BUFFERSIZE);
                clientReceive(&clientSock, buffer); // receive message from server

                printf("\nServer Message: %s\n", buffer); // display message to client
            }
            else { // if it's a seller editing
                memset(buffer, 0, BUFFERSIZE);   // clear any potential leftover memory in buffer

                strcat(buffer, "[EDIT_INFO],[SELLER],");
                strcat(buffer, &sellerID);
                strcat(buffer, ",[ADDRESS],");
                strcat(buffer, address);
                strcat(buffer, ",");

                clientSend(&clientSock, buffer); // send message to server
                memset(buffer, 0, BUFFERSIZE);
                clientReceive(&clientSock, buffer); // receive message from server

                printf("\nServer Message: %s\n", buffer); // display message to client
            }
            break;
        case 4: // return to whichever menu came from
            if (type == 0)
                buyerMenu(clientSock);
            else
                sellerMenu(clientSock);
    }
    }
}

/*  function to let buyer complete their order 
    params: int clientSock
    returns: void */
void completeOrder(int clientSock) {
    memset(buffer, 0, BUFFERSIZE); // clear any potential leftover memory in buffer

    strcat(buffer, "[COMPLETE_ORDER],");
    strcat(buffer, &buyerID);
    strcat(buffer, ",");
    strcat(buffer, &productID);
    strcat(buffer, ",");
    strcat(buffer, quantity);
    strcat(buffer, price);

    clientSend(&clientSock, buffer); // send message to server
    memset(buffer, 0, BUFFERSIZE);
    clientReceive(&clientSock, buffer); // receive message from server

    printf("\nServer Message: %s\n", buffer); // display message to client
}

/*  function to let seller view products
    params: int clientSock, int type
    returns: void */
void viewProducts(int clientSock, int type) {
    memset(buffer, 0, BUFFERSIZE); // clear any potential leftover memory in buffer

    if(type == 0) { // if it's a buyer
        memset(buffer, 0, BUFFERSIZE); // clear any potential leftover memory in buffer
        strcat(buffer, "[VIEW_PRODUCTS],[BUYER],");
    }
    else{
        strcat(buffer, "[VIEW_PRODUCTS],[SELLER],");
        strcat(buffer, &sellerID);
        strcat(buffer, ",");
    }

    clientSend(&clientSock, buffer); // send message to server
    memset(buffer, 0, BUFFERSIZE);
    clientReceive(&clientSock, buffer); // receive message from server

    printf("\nServer Message: %s\n", buffer); // display message to client
}

/*  function for buyer to add product to their order
    params: int clientSock
    returns: void */
void addProduct(int clientSock) {
    memset(buffer, 0, BUFFERSIZE);

    strcat(buffer, "[CHECK_BUYER],");
    strcat(buffer, &buyerID);
    strcat(buffer, ",");
    strcat(buffer, &orderID);
    strcat(buffer, ",");

    clientSend(&clientSock, buffer); // send message to server
    memset(buffer, 0, BUFFERSIZE);
    clientReceive(&clientSock, buffer); // receive message from server

    printf("\nServer Message: %s\n", buffer); // display message to client
}

/*  function for seller to add new product to list 
    params: int clientSock
    returns: void */
void addNewProduct(int clientSock) {
    // initialize storage values to be read
    char product[100];

    // get product name
    printf("Enter Product Name: ");
    scanf(" %[^\n]", product);

    // get product ID
    printf("Enter Product ID: ");
    scanf(" %[^\n]", &productID);

    // get quantity amount
    printf("Enter Quantity: ");
    scanf(" %[^\n]", quantity);

    // get price amount 
    printf("Enter Price/Unit: ");
    scanf(" %[^\n]", price);

    memset(buffer, 0, BUFFERSIZE); // clear any potential leftover memory in buffer

    strcat(buffer, "[NEW_PRODUCT],");
    strcat(buffer, &sellerID);
    strcat(buffer, ",");
    strcat(buffer, product);
    strcat(buffer, ",");
    strcat(buffer, quantity);
    strcat(buffer, ",");
    strcat(buffer, price);
    strcat(buffer, ",");

    clientSend(&clientSock, buffer); // send message to server
    memset(buffer, 0, BUFFERSIZE);
    clientReceive(&clientSock, buffer); // receive message from server

    printf("\nServer Message: %s\n", buffer); // display message to client
}

/*  function to delete product from database
    params: int clientSock
    returns: void */
void deleteProduct(int clientSock) {
    char ID;

    printf("Enter ProductID to Remove: ");
    scanf("%c", &ID);

    memset(buffer, 0, BUFFERSIZE); // clear any potential leftover memory in buffer

    strcat(buffer, "[DELETE_PROD],");
    strcat(buffer, &sellerID);
    strcat(buffer, ",");
    strcat(buffer, &ID);
    strcat(buffer, ",");

    clientSend(&clientSock, buffer); // send message to server
    memset(buffer, 0, BUFFERSIZE);
    clientReceive(&clientSock, buffer); // receive message from server

    printf("\nServer Message: %s\n", buffer); // display message to client
}

/*  function to modify the quantity available of an item
    params: int clientSock
    returns: void */
void modifyQuantity(int clientSock) {
    char ID;
    char quantitySet;

    printf("Enter ProductID to Adjust: ");
    scanf("%c", &ID);

    printf("Enter quantity to set it to: ");
    scanf("%c", &quantitySet);

    memset(buffer, 0, BUFFERSIZE); // clear any potential leftover memory in buffer

    strcat(buffer, "[MOD_QUANT],");
    strcat(buffer, &sellerID);
    strcat(buffer, ",");
    strcat(buffer, &ID);
    strcat(buffer, ",");
    strcat(buffer, &quantitySet);
    strcat(buffer, ",");

    clientSend(&clientSock, buffer); // send message to server
    memset(buffer, 0, BUFFERSIZE);
    clientReceive(&clientSock, buffer); // receive message from server

    printf("\nServer Message: %s\n", buffer); // display message to client
}

/*  function to modify the price of an item
    params: int clientSock
    returns: the new price of the item (void) */
void modifyPrice(int clientSock) {
    char ID;
    char priceSet;

    printf("Enter ProductID to Adjust: ");
    scanf("%c", &ID);

    printf("Enter price to set it to: ");
    scanf(" %[^\n]", &priceSet);

    memset(buffer, 0, BUFFERSIZE); // clear any potential leftover memory in buffer

    strcat(buffer, "[MOD_PRICE],");
    strcat(buffer, &sellerID);
    strcat(buffer, ",");
    strcat(buffer, &ID);
    strcat(buffer, ",");
    strcat(buffer, &priceSet);
    strcat(buffer, ",");

    clientSend(&clientSock, buffer); // send message to server
    memset(buffer, 0, BUFFERSIZE);
    clientReceive(&clientSock, buffer); // receive message from server

    printf("\nServer Message: %s\n", buffer); // display message to client
}

// main function to test functions
int main() {
    struct sockaddr_in server, client;
    // create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("\nError creating socket\n");
        return -1;
    }

    memset(&server, 0, sizeof(server)); // clear memory space of server

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

    // begin the program for client
    initialMenu(clientSocket);

    // close the socket when finished
    close(clientSocket);
}
