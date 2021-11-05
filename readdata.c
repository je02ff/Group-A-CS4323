#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
/*these paremeters are mostly there for abstraction later. if you change the table name 
that is being sent as testing.txt(on line 59) by default(the file i have is called testing.txt so just 
send the file you are reading from on the function call) the only perameter that really
matters is the personID one currently as that is what is being checked for viewing
it only gets one entry currently as it is all based on the personID which is a pKey

currently it only works if the ID length is four that can get altered to be more general later
example of an entry: 8432,Josh Sills,(546)415-8723,123 funny street
so if you entry 8432 as the id you wish to view that whole entry will be sent back in the result pointer
so the client can then print it out
*/
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
//main to run getting input and outputting the requested data
int main(){
   char input[100];
   char result[100] = "Results: ";
   printf("Please input the seller ID you want to see: ");
   scanf("%s", input);
   readDatabase("seller", "testing.txt", input, result);
   printf("%s\n",result);
   return 0;
}
