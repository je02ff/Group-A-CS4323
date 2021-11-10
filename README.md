# Group A CS4323 Final Project

## Progress Report
***Note: This project is not currently finished or in a working state, this is only in a state to show current progress***

The goal of this project is to demonstrate client-server communication between clients (both buyers and sellers) and the database that holds data (such as unique IDs for products and clients, as well as prices and quantity available).

Although there is one primary database server, there will be multiple sub-servers that clients will connect to, in which these sub-servers will perform the necessary communication with the primary database in order to update information across the entire system where necessary.

### Client-Server Portion: 
***To run this file individually (if needed), use command 'gcc -o clientServer clientServer.c -lpthread'***

Currently, the clientServer.c file contains what the client will see when accessing the program. It begins by asking the client if they would like to use the program as a buyer or seller.
Once the option is chosen, it will display login options, which are either to register as a new buyer/seller, or login with an ID that they already have. 
Currently, only option 1 has been partially implemented, as it will ask the user for their name, contact number, and address. What will later be implemented is writing all of this info to the server to store, so that the information can be shared with the other functions.
Once the information is given, depending on which type of client it is, will display the proper menu. This is where the current scope of the client-server portion ends, as the functions are currently being implemented to properly load necessary data into buffer for proper communication between the client and server. 
Essentially, the menu display has been fully implemented for the most part, but the functions tied to the list of options still need to be properly created. 
