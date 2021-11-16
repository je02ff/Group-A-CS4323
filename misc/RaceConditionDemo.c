#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 0x1100

//the code for fixing the race condition
union semun{
  int val;
  struct semid_ds *buffer;
  unsigned short *arr;
};

struct sembuf p = {0,-1,SEM_UNDO};
struct sembuf v = {0,+1,SEM_UNDO};

int id;

//read in the file value subtracts one and then writes back the new value
void *fileWork(void *value){
   int x;
   //this is the "wait" functions
   semop(id,&p,1);
   
   FILE* fl1 = fopen("nuumber.txt","r");
   fscanf(fl1, "%d", &x);
   fclose(fl1);
   x--;
   printf("%d\n",x);
   
   //i have these two seperate file opens because the writing back was being weird functions as it should however
   fl1 = fopen("nuumber.txt","w");
   fprintf(fl1, "%d", x);
   fclose(fl1);
   //this is the signal function
   semop(id,&v,1);
}

int main(){
   //makes the id for the semophore
   id = semget(KEY,1,0666 | IPC_CREAT);
   union semun u;
   u.val = 1;
   if(semctl(id,0,SETVAL,u) < 0){
      perror("Issue with cemctl");
      exit(1);
   }
   //makes the threads and has the threads join the functions
   pthread_t thread1;
   pthread_t thread2;
   pthread_create(&thread1, NULL, fileWork, NULL);
   pthread_create(&thread2, NULL, fileWork, NULL);
   pthread_join(thread1,NULL);
   pthread_join(thread2,NULL);
   return 0;
}
//end of fixed race condition program

//the Demo code for the race condition
/*void *fileWork(void *value){
   int x;
   FILE* fl1 = fopen("nuumber.txt","r");
   fscanf(fl1, "%d", &x);
   fclose(fl1);
   x--;
   printf("%d\n",x);
   fl1 = fopen("nuumber.txt","w");
   fprintf(fl1, "%d", x);
   fclose(fl1);
}

int main(){
   //int test = 10;
   pthread_t thread1;
   pthread_t thread2;
   pthread_create(&thread1, NULL, fileWork, NULL);
   pthread_create(&thread2, NULL, fileWork, NULL);
   pthread_join(thread1,NULL);
   pthread_join(thread2,NULL);
   return 0;
}*/
//end of race condition code
