# Group A CS4323 Final Project

In order to compile the DataServer:

In order to compile the Server: 

In order to compile the Client: "gcc -o client client.c -lpthread"

***Note: The programs must be ran in the following order - 1. DataServer 2. Server 3. Client in order to work***

OVERVIEW: The goal of this project is to demonstrate client-server communication between clients (both buyers and sellers) and the database that holds data (such as unique IDs for products and clients, as well as prices and quantity available), by emulating how an online shopping experience may behave. 

Although there is one primary database server, there will be multiple sub-servers that clients will connect to, in which these sub-servers will perform the necessary communication with the primary database in order to update information across the entire system where necessary.

### CLIENT: 
The Client portion of the program is what the user will interact with in order to accomplish their task, as either a buyer or seller. The program begins by having the user continue as either a buyer or seller, and the menu that will be displayed is dependent on this.
Once this is determined, the client will be prompted to either login or create a new account. 
If creating a new account, the program will ask for the users info (name, phone number, address, city, state, zip code) and send it to the server to add to list of users.
If the login section is chosen, the program will prompt the user to enter their credentials. 
Once logged in, the primary menu will be shown (either the buyer menu or seller menu depending on which was chosen initially). 
From here, the client will be able to perform actions that lead to either making an order or creating products, depending on the client type. 
From each of these options, the client program will be in constant communication with the server it is connected to, and the actions that are taken by each option in the menus are done by sending commands from the client to the server, and the server will take whatever command given and perform the task indicated. 
This is a summary of what the client portion entails, which is essentially a way to gather information from the client and send said information to the sub-server, which will update the functions in the data server accordingly.

### Server Side:
***To run server side, use command 'gcc -o dataServer dataServer.c -lpthread'***
