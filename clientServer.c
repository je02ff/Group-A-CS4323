/*  
    Group A
    Author: Jeremiah Pete
    Email: jeremiah.pete@okstate.edu

    Program Description: This file handles the structuring of the client side of the program,
    both the buyer and seller   */

#include "clientServer.h"

/*  function to display and handle the initial menu of the program
    params: none 
    returns: void */
void initialMenu() {
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
                buyerLogin();
            case 2: // go to sellerLogin()
                sellerLogin();
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
void buyerLogin() {
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
                userRegister(type);
            case 2: // login with customer ID
                //IDLogin();
            case 3: // go back to main menu
                //system("clear");
                initialMenu();
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
void sellerLogin() {
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
                userRegister(type);
            case 2: // login with sellerID
                //sellerIDLogin();
            case 3: // go back to the initial menu
                //system("clear");
                initialMenu();
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
void userRegister(int type) {
    // declare variables to store info needed for registering users
    char name[100];
    char num[50];
    char address[100];

    if (type == 0) { // if it's a buyer registering
        // get name info
        printf("Please Enter Name: "); 
        scanf(" %[^\n]", name);
        printf("\nHello, %s!", name); // return name to make sure input is correct

        // get number info
        printf("\n\nPlease Enter Contact Number: "); 
        scanf(" %[^\n]", num);
        printf("\nNumber Entered: %s", num); // return number to make sure input is correct

        // get address info
        printf("\n\nPlease Enter Address (Format: Street Name, City, State, Zip Code): "); 
        scanf(" %[^\n]", address);
        printf("\nAddress Entered: %s", address); // return address to make sure input is correct

        /** TODO: Write info to client server **/

        buyerMenu(); // go to buyer menu
    }
    else { // if it's a seller registering
        // get name info
        printf("Please Enter Name: "); 
        scanf(" %[^\n]", name);
        printf("\nHello, %s!", name); // return name to make sure input is correct

        // get number info
        printf("\n\nPlease Enter Contact Number: ");
        scanf(" %[^\n]", num);
        printf("\nNumber Entered: %s", num); // return number to make sure input is correct

        // get address info
        printf("\n\nPlease Enter Address (Format: Street Name, City, State, Zip Code): "); 
        scanf(" %[^\n]", address);
        printf("\nAddress Entered: %s", address); // return address to make sure input is correct

        /** TODO: Write info to client server **/

        sellerMenu(); // go to seller menu
    }
}

/*  function to handle the login of the user (both buyer and seller)
    params: int type (to distinct between buyer and seller)
    returns: void */
void IDLogin(int type) {
    /** TODO: Perform a check to the database that determines if the credentials are valid or not **/
}

/*  function to handle the buyer menu model once successfully logged in
    params: none
    returns: void */
void buyerMenu() {
    int select;

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
                //makeOrder();
            case 2: // view orders
                //viewOrder();
            case 3: // modify order
                //modifyOrder();
            case 4: // view billing info
                //viewBill();
            case 5: // edit info
                //editInfo();
            case 6: // back to main menu
                initialMenu();
            case 7: // exit case
                exit(0);
            default:
                printf("Please select a valid choice!\n\n");
        }
    }
}

/*  function to handle the seller menu model once successfully logged in
    params: none
    returns: void */
void sellerMenu() {
    int select;

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
                //viewProducts();
            case 2: // add new product 
                //addProduct();
            case 3: // delete product
                //deleteProduct();
            case 4: // modify quantity
                //modifyQuantity();
            case 5: //  modify price
                //modifyPrice();
            case 6: // view product orders
                //viewOrder();
            case 7: // edit seller info
                //editInfo();
            case 8: // return to main menu
                initialMenu();
            case 9: // exit case
                printf("Program Terminated\n");
                exit(0);
            default:
                printf("Please select a valid choice!\n\n");
        }
    }
}

/*** TODO: functions that still need to be implemented ***/
void makeOrder();
void viewOrder();
void modifyOrder();
void viewBill();
void editInfo();
void viewProducts();
void addProduct();
void deleteProduct();
void modifyQuantity();
void modifyPrice();

// main function to test functions
int main() {
    initialMenu();
}