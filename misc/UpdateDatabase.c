#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
/*
this kinda works. one last formatting bug needs to be fixed. im too tired to actually make it work currently
*/
int updateDatabase(char personID[4]){
   char buffer;
   char row[100];
   char helper[100];
   char reinput[1000];
   int position = 0;
   int numCommas = 0;
   char ID[4];
   int checker = 0;
   //opens the file buffer
   FILE* fl1 = fopen("testing.txt", "r+");
   buffer = fgetc(fl1);
   //loops through every char in the file until the end
   while(buffer != EOF){
   //printf("position: %d\n", position);
      if(buffer == '\n' || position == 0){
      //printf("IN IF\n");
      if(position == 0){
         ID[0] = buffer;
         reinput[0] = buffer;
         buffer = fgetc(fl1);
         ID[1] = buffer;
         //position++;
         reinput[1] = buffer;
         buffer = fgetc(fl1);
         ID[2] = buffer;
         //position++;
         reinput[2] = buffer;
         buffer = fgetc(fl1);
         ID[3] = buffer;
         //position++;
         reinput[3] = buffer;
         position = position + 3;
      }
      else{
         reinput[position] = '\n';
         
         buffer = fgetc(fl1);
         ID[0] = buffer;
         position++;
         reinput[position] = buffer;
         
         buffer = fgetc(fl1);
         ID[1] = buffer;
         position++;
         reinput[position] = buffer;
         
         buffer = fgetc(fl1);
         ID[2] = buffer;
         position++;
         reinput[position] = buffer;
         
         buffer = fgetc(fl1);
         ID[3] = buffer;
         position++;
         reinput[position] = buffer;
         //position = position + 5;
      }
         printf("\nID:%s\n",ID);
         //strcat(reinput,ID);
         printf("%s\n", reinput);
         if(strcmp(ID,personID) == 0){
            printf("Insert new name: ");
            memset(row, 0, strlen(row));
            //scanf("%s\n", row);
            fgets(row, 100, stdin);
            strcat(reinput,",");
            strncpy(row,row,strlen(row) -2);
            printf("\nROW:%s\n",row);
            strcat(reinput,row);
            reinput[strlen(reinput)-1] = 0;
            
            printf("Insert new Phone Number in form of (xxx)xxx-xxxx: ");
            memset(row, 0, strlen(row));
            //scanf("%s\n", row);
            fgets(row, 100, stdin);
            strcat(reinput,",");
            //strncpy(row,row,strlen(row) -2);
            printf("\nROW:%s\n",row);
            strcat(reinput,row);
            reinput[strlen(reinput)-1] = 0;
            
            printf("Insert new Address: ");
            memset(row, 0, strlen(row));
            //scanf("%s\n", row);
            fgets(row, 100, stdin);
            strcat(reinput,",");
            strncpy(row,row,strlen(row) -2);
            printf("\nROW:%s\n",row);
            strcat(reinput,row);
            
            strcat(reinput,"\n");
            position = strlen(reinput)-1;
            buffer = fgetc(fl1);
            
            while(buffer != '\n'){
            //printf("\nCHAR NEXT%c\n", buffer);
            buffer = fgetc(fl1);
            }
            memset(ID, 0, 4);
         }
      }
      else{
      printf("%c",buffer);
         reinput[position] = buffer;
         position++;
      }
      buffer = fgetc(fl1);
   }
   //fprintf(fl1, "%s", reinput);
   printf("\n\n\n%s", reinput);
   fclose(fl1);
}
//main to run getting input and outputting the requested data
int main(){
   char input[100];
   char result[100] = "Results: ";
   //printf("Please input the : ");
   //scanf("%s", input);
   updateDatabase("3217");
   //printf("%s\n",result);
   return 0;
}
