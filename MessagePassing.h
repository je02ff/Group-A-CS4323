/*  Jeremiah Pete
    Header file for my functions    */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

// define functions
int menu(int socket_client); // menu function at the beginning of program
int messagePass(pid_t pid, int socket_client); // to handle message passing requirements

int accessPBThreadManage(char message[], int socket_client); // access the necessary info from Kyle's file

char* getMessage(char message[]); // retrieve user message 
char* lowerCase(char message[], char temp[]); // convert message to all lower case