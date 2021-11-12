#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "Roden.h"
#include "readDatabaseIntoArray.h"

//takes in the int to be changed and a pointer for a char array to place the charcters into 
int intToCharArray(int change, char* result){
int temp = change;
int position = 0;
while(temp != 0){
	//switch that takes a single digit from the int and adds its complimentary char 
	//into a result array starting from the one's digit and building up
   switch (temp%10){
      case 0:
         result[position] = '0';
         position++;
         break;
      case 1:
         result[position] = '1';
         position++;
         break;
      case 2:
         result[position] = '2';
         position++;
         break;
      case 3:
         result[position] = '3';
         position++;
         break;
      case 4:
         result[position] = '4';
         position++;
         break;
      case 5:
         result[position] = '5';
         position++;
         break;
      case 6:
	 result[position] = '6';
	 position++;
 	 break;
      case 7:
	 result[position] = '7';
	 position++;
	 break;
      case 8:
	 result[position] = '8';
	 position++;
	 break;
      case 9:
	 result[position] = '9';
	 position++;
	 break;
      default:
	 break;
   }
   temp = temp/10;
}
//the input with the method is backwards so this reverses the string to be in the proper order
   int pos = strlen(result)-1;
   char rev[strlen(result)];
   for(int x = 0; x < strlen(result); x++){
   rev[x] = result[pos];
   pos--;
   }
   strncpy(result,rev,strlen(result));
}
//takes in the product info struct and writes it back to the file ProductInfo.txt
int writeBackProductStruct(struct csvProductInfo table[]){

char putBack[1000];
char result[100];
//goes through each row in the database currently set to 8 but can and will be changed when merge
for(int x = 0; x < 8; x++){
memset(result, 0, strlen(result));
//have to call intToCharArray for strcat
intToCharArray(table[x].productId,result);
strcat(putBack, result);
memset(result, 0, strlen(result));
strcat(putBack, ",");

strcat(putBack, table[x].productName);
strcat(putBack, ",");

intToCharArray(table[x].sellerId,result);
strcat(putBack, result);
memset(result, 0, strlen(result));
strcat(putBack, ",");

intToCharArray(table[x].quantity,result);
strcat(putBack, result);
memset(result, 0, strlen(result));
strcat(putBack, ",");

intToCharArray(table[x].price,result);
strcat(putBack, result);
memset(result, 0, strlen(result));
if(x !=7){
strcat(putBack, ",\n");
}
}

FILE* fl1 = fopen("ProductInfo.txt","w+");
fprintf(fl1,"%s",putBack);
fclose(fl1);
return 0;
}
//TODO add the write backs for the other databases
