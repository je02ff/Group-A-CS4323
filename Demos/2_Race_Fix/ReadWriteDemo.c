#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdbool.h>

#define KEY 0x1100
#define KEY2 0x1100

union semun{
  int val;
  struct semid_ds *buffer;
  unsigned short *arr;
};

struct sembuf p = {0,-1,SEM_UNDO};
struct sembuf v = {0,+1,SEM_UNDO};

int id;
int numReader;
bool readOpen;

//FIXED CODE
void *reader(void *value){
   int x;
   numReader++;
   
   //first reader needs to get the lock and tell the other readers is ok to go on
   if(numReader == 1){
   //printf("IN IF \n");
   semop(id,&p,1);
   //printf("AFTER LOCK \n");
   readOpen = true;
   }
   printf("READER NUMBER %d\n", numReader);
   
   //makes readers that are not the first wait until the first reader gets the lock
   while(!readOpen){}
   //printf("AFTER WHILE \n");
   FILE* fl2 = fopen("bill.txt","r");
   fscanf(fl2, "%d", &x);
   printf("READING: %d\n", x);
   numReader--;
   fclose(fl2);
   //gives job to the last reader to release the lock and turn off "reading" mode so to speak
   if(numReader == 0){
   semop(id,&v,1);
   readOpen = false;
   }
}

void *writer(void *value){
   int x;
   //printf("BEFORE LOCK\n");
   semop(id,&p,1);
   FILE* fl2 = fopen("bill.txt","r");
   fscanf(fl2, "%d", &x);
   fclose(fl2);
   printf("AFTER READ IN THE WRITE: %d\n", x);
   x++;
   fl2 = fopen("bill.txt","w");
   fprintf(fl2, "%d", x);
   fclose(fl2);
   semop(id,&v,1);
   printf("RESULT WRITING BACK: %d\n",x);
}
//END OF FIXED CODE

//BROKEN CODE
/*void *reader(void *value){
   int x;
   numReader++;
  
   printf("READER NUMBER %d\n", numReader);

   FILE* fl2 = fopen("bill.txt","r");
   fscanf(fl2, "%d", &x);
   printf("READING: %d\n", x);
   numReader--;
   fclose(fl2);

}

void *writer(void *value){
   int x;

   FILE* fl2 = fopen("bill.txt","r");
   fscanf(fl2, "%d", &x);
   fclose(fl2);
   printf("AFTER READ IN THE WRITE: %d\n", x);
   x++;
   fl2 = fopen("bill.txt","w");
   fprintf(fl2, "%d", x);
   fclose(fl2);

   printf("RESULT WRITING BACK: %d\n",x);
}*/
//END OF BROKEN CODE
int main(){
  id = semget(KEY,1,0666 | IPC_CREAT);
  readOpen = false;
  union semun u;
  u.val = 1;
  numReader = 0;
   //keep track of the number of readers 
   if(semctl(id,0,SETVAL,u) < 0){
      perror("Issue with cemctl");
      exit(1);
   }
   //these threads simulate multiple clients one writer and 4 readers and i have them come in at a "weird" order
   pthread_t thread1;
   pthread_t thread3;
   pthread_t thread4;
   pthread_t thread5;
   pthread_t thread6;
   pthread_t thread2;
   pthread_create(&thread1, NULL, reader, NULL);
   pthread_create(&thread2, NULL, writer, NULL);
   pthread_create(&thread3, NULL, reader, NULL);
   pthread_create(&thread4, NULL, reader, NULL);
   pthread_create(&thread5, NULL, reader, NULL);
   pthread_create(&thread6, NULL, writer, NULL);
   pthread_join(thread1,NULL);
   pthread_join(thread2,NULL);
   pthread_join(thread3,NULL);
   pthread_join(thread4,NULL);
   pthread_join(thread5,NULL);
   pthread_join(thread6,NULL);
  
  return 0;
}
