// Header file for client.c functions
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// menu functions
void initialMenu();
void buyerLogin();
void buyerMenu();
void sellerLogin();
void sellerMenu();
void userRegister(int type);
void IDLogin(int type);

// info handling functions
void makeOrder();
void viewOrder();
void modifyOrder();
void viewBill();
void editInfo();
void completeOrder();
void viewProducts();
void addProduct();
void addNewProduct();
void sellerMenu();
void deleteProduct();
void modifyQuantity();
void modifyPrice();