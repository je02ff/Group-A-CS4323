#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 0x1100
#define KEY2 0x1100

//code that fixes the deadlock
union semun{
  int val;
  struct semid_ds *buffer;
  unsigned short *arr;
};

struct sembuf p = {0,-1,SEM_UNDO};
struct sembuf v = {0,+1,SEM_UNDO};

int id;
int id2;

void *buy(void *value){
   int x;
   //wait for the lock for prod
   semop(id,&p,1);
   
   FILE* fl1 = fopen("prod.txt","r");
   fscanf(fl1, "%d", &x);
   fclose(fl1);
   x--;
   
   fl1 = fopen("prod.txt","w");
   fprintf(fl1, "%d", x);
   fclose(fl1);
   
   //release the lock for prod
   semop(id,&v,1);

   //wait for the lock for bill
   semop(id2,&p,1);

   fl1 = fopen("bill.txt","r");
   fscanf(fl1, "%d", &x);
   fclose(fl1);
   x++;
   
   fl1 = fopen("bill.txt","w");
   fprintf(fl1, "%d", x);
   fclose(fl1);
   //release the lock for bill
   semop(id2,&v,1);
}

void *ret(void *value){
   int x;
   //wait to get the lock for the bill file
   semop(id2,&p,1);

   FILE* fl2 = fopen("bill.txt","r");
   fscanf(fl2, "%d", &x);
   fclose(fl2);
   x--;
   
   fl2 = fopen("bill.txt","w");
   fprintf(fl2, "%d", x);
   fclose(fl2);
   
   //release the lock for the bill file
   semop(id2,&v,1);
   
   //wait to get the lock for prod
   semop(id,&p,1);

   fl2 = fopen("prod.txt","r");
   fscanf(fl2, "%d", &x);
   fclose(fl2);
   x++;
   
   fl2 = fopen("prod.txt","w");
   fprintf(fl2, "%d", x);
   fclose(fl2);
   
   //releae the lock for prod
   semop(id,&v,1);
}

int main(){
  id = semget(KEY,1,0666 | IPC_CREAT);
  id2 = semget(KEY2,1,0666 | IPC_CREAT);
  
   union semun u;
   union semun l;
   u.val = 1;
   l.val = 1;
   
   if(semctl(id,0,SETVAL,u) < 0){
      perror("Issue with cemctl");
      exit(1);
   }
   if(semctl(id,0,SETVAL,l) < 0){
      perror("Issue with cemctl");
      exit(1);
   }
  
   pthread_t thread1;
   pthread_t thread2;
   pthread_create(&thread1, NULL, buy, NULL);
   pthread_create(&thread2, NULL, ret, NULL);
   pthread_join(thread1,NULL);
   pthread_join(thread2,NULL);
  
  return 0;
}

//code that has the deadlock(hold and wait i think) 
//the functions get the locks they need at the time and release all of them at the end
//this leads to deadlock as the functions get the locks in different orders but to finish need all of them

/*union semun{
  int val;
  struct semid_ds *buffer;
  unsigned short *arr;
};

struct sembuf p = {0,-1,SEM_UNDO};
struct sembuf v = {0,+1,SEM_UNDO};

int id;
int id2;

void *buy(void *value){
   int x;
   semop(id,&p,1);
   FILE* fl1 = fopen("prod.txt","r");
   fscanf(fl1, "%d", &x);
   fclose(fl1);
   x--;
   fl1 = fopen("prod.txt","w");
   fprintf(fl1, "%d", x);
   fclose(fl1);
   
   semop(id2,&p,1);
   fl1 = fopen("bill.txt","r");
   fscanf(fl1, "%d", &x);
   fclose(fl1);
   x++;
   fl1 = fopen("bill.txt","w");
   fprintf(fl1, "%d", x);
   fclose(fl1);
   semop(id,&v,1);
   semop(id2,&v,1);
}

void *ret(void *value){
   int x;
   
   semop(id2,&p,1);
   FILE* fl2 = fopen("bill.txt","r");
   fscanf(fl2, "%d", &x);
   fclose(fl2);
   x--;
   fl2 = fopen("bill.txt","w");
   fprintf(fl2, "%d", x);
   fclose(fl2);

   semop(id,&p,1);
   fl2 = fopen("prod.txt","r");
   fscanf(fl2, "%d", &x);
   fclose(fl2);
   x++;
   fl2 = fopen("prod.txt","w");
   fprintf(fl2, "%d", x);
   fclose(fl2);
   
   semop(id,&v,1);
   semop(id2,&v,1);
}

int main(){
  id = semget(KEY,1,0666 | IPC_CREAT);
  id2 = semget(KEY2,1,0666 | IPC_CREAT);
  
   union semun u;
   union semun l;
   u.val = 1;
   l.val = 1;
   
   if(semctl(id,0,SETVAL,u) < 0){
      perror("Issue with cemctl");
      exit(1);
   }
   if(semctl(id,0,SETVAL,l) < 0){
      perror("Issue with cemctl");
      exit(1);
   }
  
   pthread_t thread1;
   pthread_t thread2;
   pthread_create(&thread1, NULL, buy, NULL);
   pthread_create(&thread2, NULL, ret, NULL);
   pthread_join(thread1,NULL);
   pthread_join(thread2,NULL);
  
  return 0;
}*/
