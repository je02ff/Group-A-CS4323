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

// menu functions
void initialMenu();
void buyerLogin();
void buyerMenu();
void sellerLogin();
void sellerMenu();
void userRegister(int type);
void IDLogin(int type);