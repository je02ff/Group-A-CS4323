#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "Roden.h"
#include "readDatabaseIntoArray.h"

int intToCharArray(int change, char* result){
int temp = change;
int position = 0;
while(temp != 0){
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
   int pos = strlen(result)-1;
   char rev[strlen(result)];
   for(int x = 0; x < strlen(result); x++){
   rev[x] = result[pos];
   pos--;
   }
   strncpy(result,rev,strlen(result));
}

int writeBackProductStruct(struct csvProductInfo table[]){
//FILE* fl1 = fopen("ProductInfo.txt","w+");
   char putBack[1000];
   char result[100];
   for(int x = 0; x < 8; x++){
	memset(result, 0, strlen(result));

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
   //printf("%s",putBack);
   FILE* fl1 = fopen("ProductInfo.txt","w+");
   fprintf(fl1,"%s",putBack);
   fclose(fl1);
   return 0;
}

int writeBackBillingStruct(struct csvBillingInfo table[]){
   //FILE* fl1 = fopen("ProductInfo.txt","w+");
   char putBack[1000];
   char result[100];
   for(int x = 0; x < 4; x++){
	memset(result, 0, strlen(result));

	intToCharArray(table[x].orderId,result);
	strcat(putBack, result);
	memset(result, 0, strlen(result));
	strcat(putBack, ",");

	intToCharArray(table[x].customerId,result);
	strcat(putBack, result);
	memset(result, 0, strlen(result));
	strcat(putBack, ",");

	strcat(putBack, table[x].firstName);
	strcat(putBack, ",");

	strcat(putBack, table[x].lastName);
	strcat(putBack, ",");

	strcat(putBack, table[x].streetAddress);
	strcat(putBack, ",");

	strcat(putBack, table[x].city);
	strcat(putBack, ",");

	strcat(putBack, table[x].state);
	strcat(putBack, ",");

	strcat(putBack, table[x].zipCode);
	strcat(putBack, ",");

	intToCharArray(table[x].totalOrderCost,result);
	strcat(putBack, result);
	memset(result, 0, strlen(result));
	strcat(putBack, ",");

	if(x !=4){
	   strcat(putBack, "\n");
	}
   }
   //printf("%s",putBack);
   FILE* fl1 = fopen("BillingInfo.txt","w+");
   fprintf(fl1,"%s",putBack);
   fclose(fl1);
   return 0;
}

int writeBackClientStruct(char* db, struct csvClientInfo table[]){
   //FILE* fl1 = fopen("ProductInfo.txt","w+");
   char putBack[1000];
   char result[100];
   for(int x = 0; x < 4; x++){
	memset(result, 0, strlen(result));

	intToCharArray(table[x].uuid,result);
	strcat(putBack, result);
	memset(result, 0, strlen(result));
	strcat(putBack, ",");

	strcat(putBack, table[x].firstName);
	strcat(putBack, ",");

	strcat(putBack, table[x].lastName);
	strcat(putBack, ",");

	strcat(putBack, table[x].phoneNumber);
	strcat(putBack, ",");

	strcat(putBack, table[x].streetAddress);
	strcat(putBack, ",");

	strcat(putBack, table[x].city);
	strcat(putBack, ",");

	strcat(putBack, table[x].state);
	strcat(putBack, ",");

	strcat(putBack, table[x].zipCode);
	strcat(putBack, ",");

	if(x !=4){
	   strcat(putBack, "\n");
	}
   }
   //printf("%s",putBack);
   FILE* fl1 = fopen(db,"w+");
   fprintf(fl1,"%s",putBack);
   fclose(fl1);
   return 0;
}

int writeBackCustomerOrderStruct(struct csvCustomerOrderInfo table[]){
   //FILE* fl1 = fopen("ProductInfo.txt","w+");
   char putBack[1000];
   char result[100];
   for(int x = 0; x < 7; x++){
	memset(result, 0, strlen(result));

	intToCharArray(table[x].orderId,result);
	strcat(putBack, result);
	memset(result, 0, strlen(result));
	strcat(putBack, ",");

	intToCharArray(table[x].productId,result);
	strcat(putBack, result);
	memset(result, 0, strlen(result));
	strcat(putBack, ",");

	intToCharArray(table[x].quantityPurchased,result);
	strcat(putBack, result);
	memset(result, 0, strlen(result));
	strcat(putBack, ",");

	strcat(putBack, table[x].firstName);
	strcat(putBack, ",");

	strcat(putBack, table[x].lastName);
	strcat(putBack, ",");

	strcat(putBack, table[x].streetAddress);
	strcat(putBack, ",");

	strcat(putBack, table[x].city);
	strcat(putBack, ",");

	strcat(putBack, table[x].state);
	strcat(putBack, ",");

	strcat(putBack, table[x].zipCode);
	strcat(putBack, ",");

	intToCharArray(table[x].totalPrice,result);
	strcat(putBack, result);
	memset(result, 0, strlen(result));
	strcat(putBack, ",");

	if(x !=7){
	   strcat(putBack, "\n");
	}
   }
   //printf("%s",putBack);
   FILE* fl1 = fopen("CustomerOrder.txt","w+");
   fprintf(fl1,"%s",putBack);
   fclose(fl1);
   return 0;
}
