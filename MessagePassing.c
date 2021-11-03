/*  
    Group C
    Author: Jeremiah Pete
    Email: jeremiah.pete@okstate.edu

    Program Description: This file handles the IPC using pipes and implements peer's files where needed
    To compile solo, uncomment the main method (since it must be compiled together with the clientDriver),  
    then use command gcc -o message MessagePassing.c -lpthread
*/
#include "MessagePassing.h"
#include "HunterHolstead.h"
#include "ProcessB_ThreadManagement.h"

pthread_cond_t WAKE = PTHREAD_COND_INITIALIZER; // Create thread condition variable
int done = 1; // for thread signal

//  function to display the menu to the client side
int menu(int socket_client) { 
    // initialize variables to handle selection of user choice
    pid_t pid; // for handling processes
    int selection; 

    printf("Main Menu:\n\nPlease select an option below -\n\n");
    // display option choices to user
    while(1) {
        printf("1. Send the data to the server\n");
        printf("2. Receive the file from the server\n");
        printf("3. Terminate\n");

        // Receive user's selection
        printf("Selection: ");
        scanf("%d", &selection);

        // upon the selection, determine which function to perform through switch
        switch (selection) {
            case 1: // go to messagePass
                messagePass(pid, socket_client);
            case 2: // receive file from server
                // Process B should be killed (as with thread T)
                
            case 3: // exit program
                printf("\nProgram Terminated\n");
                kill(0, SIGKILL); // kill all running processes
            default: // handles when user inputs value not available
                printf("\nPlease select a proper choice!\n\n");
        }
    }
}

/*  function to perform message passing in Option 1 of assignment
    Process A will be the parent function in this case, and Process B will be the child
    params: pid_t pid
    returns: menu   */
int messagePass(pid_t pid, int socket_client) {
    int q = Random(); // for encryption
    int p[2]; // p[0] is for reading (Process B), p[1] is for writing (Process A)

    char message[200]; // made the size large for longer input cases

    getMessage(message); // retrieve message user wants to send
    char* temp = lowerCase(message, temp); // make message lowercase for checking
    
    if (pipe(p) == -1) { // case when the pipe cannot be instantiate 
        fprintf(stderr, "Pipe Couldn't be Made");
        exit(2);
    }

    pid = fork(); // initialize the child Process B
    // pid > 0 is parent process, pid == 0 is child process
    if (pid > 0) { // Process A writes the message the user wants to input
        close(p[0]); // close reading end of pipe

        write(p[1], message, sizeof(message)); // write the message to the pipe and close the write end of pipe
        close(p[1]); // close writing end of pipe

        wait(NULL); // wait for child process to die
    }
    else if (pid == 0) { // Process B reads the text written in Process A
        pthread_cond_signal(&WAKE);
        struct theMessage m;
        close(p[1]); // close writing end
        // read the message made in Process A from the pipe 
        read(p[0], message, sizeof(message));
        printf("\nProcess B has received message: %s\n\n", message); // test to make sure IPC functions properly

        // use checkWord function from Hunter's file to continuously check function
        while(!checkWord(temp)) {
		    printf("\nWord %s not found; please re-enter: ", temp);
		    scanf("%s", temp);
	    } 
        // implement struct values created in ProcessB_ThreadManagement.c 
        pthread_t T; 
        m.msg = message;
        m.size = sizeof(message);
        m.key = q;
        m.mThread = &T;
        accessPBThreadManage(message, socket_client);
        close(p[0]); // close reading end

        printf("\n"); // formatting
    }
    else // if child process can't be created
        printf("Child process couldn't be created\n\n");
    return menu(socket_client); // display the menu once again when finished
}

/*  function to get message input from user
    params: char message[] (initialized in menu function)
    returns: stdin string message */
char* getMessage(char message[]) {
    printf("\nWhat message would you like to send?\n");
    printf("Message: ");
    scanf(" %[^\n]", message);
    return message;
}

/*  function to make message input all lowercase in order for checkWord to work
    params: char message[] 
    returns: all lowercase version of message (temp) */
char* lowerCase(char message[], char temp[]) {
    for (int i = 0; i < strlen(message) + 1; i++) {
        temp[i] = tolower(message[i]);
    }

    return temp;
}

/*  Create function to access ProcessB_ThreadManagement.c function coherently
    params: none
    return: int 
    utilizes Kyle's main method for basis on how to function */
int accessPBThreadManage(char message[], int socket_client) {
    pthread_mutex_t mutex; // create pthread mutex to be used by Thread T

	// Variables for handling random numbers
	time_t t;
	srand((unsigned) time(&t));

	// Get a random encryption key
	int myKey = Random();
	printf("Key generated: %d\n", myKey);

	// Test getFileName
	char* strServer = getFileName("Server_");
	printf("%s\n", strServer);
	free(strServer);
		
	// Pointer to the filename returned by the server
	char* serverFileName;
	
	// Declare a pointer to Thread T
	pthread_t* ptrThreadT = NULL;
	
	// Run-time initialization of the pthread mutex with default attributes
	if (pthread_mutex_init(&mutex, NULL) != 0) {
		return 1;
	}
	
	// Lock the mutex initially; disallow the thread from returning the file name until Process B terminates
	pthread_mutex_lock(&mutex);
	
	char* heapString = (char*) malloc(strlen(message));
	strcpy(heapString, message);
	printf("String: %s\n", heapString);
	printf("Pointer: %p\n", heapString);

	// Test manageThreads (ptrThreadT passed back by reference)
	manageThreads(&ptrThreadT, heapString, strlen(message), myKey);

	// Unlock the mutex to return the filename for the encrypted message
	printf("Unlocking mutex...\n");
	pthread_mutex_unlock(&mutex);

    if (done == 1) {
	    // Get the filename (address) from Thread T and destroy the thread (this should occur prior to terminating Process B)
        done = 2; // wait on WAKE
	    pthread_join(*ptrThreadT, (void**) &serverFileName);
	    printf("\nServer file address: %p\n", serverFileName);
    }
    else {
        printf("Signaling condition variale WAKE\n");
        pthread_cond_signal(&WAKE);
    }
    printf("\nEncrypted String: %s\n", heapString);
    printf("Pointer: %p\n", heapString);

    sendMessageToServer(socket_client, heapString, myKey);
	// Free the heap memory associated with the thread pointer
	printf("Freeing memory for pointer %p...\n", ptrThreadT);
	free(ptrThreadT);

	// Free the heap memory associated with the string pointer
	printf("Freeing memory for pointer %p...\n", heapString);
	free(heapString);
	
	// Destroy the lock
	if (pthread_mutex_destroy(&mutex) != 0) {
		return 1;
	}
	
	return 0;    
}

/*
// test case 
int main() {
    menu();
    return 0;
}  */