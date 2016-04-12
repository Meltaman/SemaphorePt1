/* Author: Andrew Cooper
*  This program was designed to demonstrate the purpose of semaphors.
*  Two child processes access a critical value in shared memory and write to it simultaneously. 
*  The goal was to see how close it could end up to 2 million without the use of semaphores.
*  I added, but commented out 2 sleep commands that would essentially delay the writing to the memory allowing
*     semaphores to be crudely emulated. These were commented out as they were not within the parameters of the
*     assignment.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

#define MAXCOUNTERVAL 1000000

typedef struct{
    int value;
}shared_mem; 

shared_mem *counter;


main(){
    //	shared memory key, ID, and allocation
    key_t key = IPC_PRIVATE;
    int shmid;
    shared_mem *shmat1;
    
    pid_t pid;	
    
    //	process ids for children
    int child1;	
    int child2;	

    int idx1 = 0;
    int idx2 = 0;

    //	attempts to attach to an existing memory segment
    if (( shmid = shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0){

        perror("shmget"); 
        exit(1);
    }

    //	attempts to access the shared memory segment
    if((counter = (shared_mem *)shmat(shmid, NULL, 0)) == (shared_mem *) -1){

        perror("shmat"); 
        exit(1);
    }

    //	initializing shared memory to 0
    counter->value = 0;

    //	fork the first process
    pid = fork();

    if (pid == -1){
        
        printf("Error creating first child.\n");
        exit(0);

    }else if (pid == 0){

        child1 = getpid();
        fprintf(stderr,"Hello from Child 1, my pid is %d\n", child1);
    
        while(idx1 < MAXCOUNTERVAL){
            counter->value = counter->value + 1;
            idx1++;
        }

        fprintf(stderr,"Child 1 (pid %d) signing off. Counter is at %d\n", child1, counter->value);
        exit(0);
    }

    // fork the second process
    pid = fork();
    if (pid == -1){
        printf("Error creating second child.\n");
        exit(0);
    }else if (pid == 0){
        child2 = getpid();

        //if you sleep here, and in the parent process below
        //you can force only one child to have access to the
        //critical shared memory value at the same time making the 
        //counter accurately hit 2 million every time.

        //sleep(5);

        fprintf(stderr,"Hello from Child 2, my pid is %d\n", child2);
    
        while(idx2 < MAXCOUNTERVAL){
            counter->value = counter->value + 1;
            idx2++;
        }	
        
        fprintf(stderr,"Child 2 (pid %d) signing off. Counter is at %d\n", child2, counter->value); 
        exit(0);
    }
    if (pid > 0){

        // parent waits for child processes
        wait(); 
        wait();	
        //if you sleep here, and in the second child process above
        //you can force only one child to have access to the
        //critical shared memory value at the same time making the
        //counter accurately hit 2 million every time.
        
        //sleep(10);

        fprintf(stderr,"I'm the parent, I have a pid of %d.\n", getpid());
        //	parent process reports value of counter
        fprintf(stderr,"Counter value is: %d\n", counter->value);
        //	parent process exits safely
        exit(0);	
    }else{
        fprintf(stderr,"Forking error\n");
    }
    
    //	deallocate shared memory
    if(shmctl(shmid, IPC_RMID, (struct shmid_ds *)0)== -1){ 
        perror("shmctl");
        exit(-1);
    }
    
exit(0);

}//	end main

    
