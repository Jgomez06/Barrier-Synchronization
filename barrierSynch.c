#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include <stdbool.h>	/* Booleans */

#define NUM_PROCESS 5
#define TOTAL_PHASES 10

/* prototype for thread routines */
void processFast ( void *ptr );
void processSlow ( void *ptr );

/* semaphores are declared global so they can be accessed in main() and in thread routine */
sem_t sem_even, sem_odd, sem_synch;
int threads_finished = 0;

int main()
{    
    /* Thread IDs */
    int i[NUM_PROCESS] = {1,2,3,4,5};
  
    /* Declare threads */ 
    pthread_t thread_1;
    pthread_t thread_2;
    pthread_t thread_3;
    pthread_t thread_4;
    pthread_t thread_5;

    /* Initialize to 0 to block on each new phase */
    sem_init(&sem_even, 0, 0);     
    sem_init(&sem_odd, 0, 0);   
    /* Initialized to 1 so only one thread can change the counter at any time */  
    sem_init(&sem_synch, 0, 1);    
 
    /* Create all the Threads*/                                 
    pthread_create (&thread_1, NULL, (void *) &processFast, (void *) &i[0]);
    pthread_create (&thread_2, NULL, (void *) &processSlow, (void *) &i[1]);
    pthread_create (&thread_3, NULL, (void *) &processFast, (void *) &i[2]);
    pthread_create (&thread_4, NULL, (void *) &processSlow, (void *) &i[3]);
    pthread_create (&thread_5, NULL, (void *) &processSlow, (void *) &i[4]);

    /* Wait for all the threads to finish executing */ 
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    pthread_join(thread_3, NULL);
    pthread_join(thread_4, NULL);
    pthread_join(thread_5, NULL);

    /* destroy semaphores */
    sem_destroy(&sem_even); 
    sem_destroy(&sem_odd);
    sem_destroy(&sem_synch);
                  
    /* exit */  
    exit(0);

} /* end main() */


void barrierSynch(int i,int x) {

  sem_wait(&sem_synch);
  // Increase the number of threads that have finished phase i
  threads_finished++;		
  printf("Process %d was #%d to finish phase %d\n", x, threads_finished, i);

  if (threads_finished == NUM_PROCESS) { 	
	// All threads have finished
	printf("\nPHASE %d is OVER\n\n", i);
	int x = 0;
	for(x; x < NUM_PROCESS-1; x++) {
		if(i%2 == 0) {
	    		sem_post(&sem_even);
		} else {
			sem_post(&sem_odd);
		}		
	}
  // Reset the count
  threads_finished = 0; 	
  sem_post(&sem_synch);
   } else {
		sem_post(&sem_synch);
		if(i%2 == 0) {
			printf("Process %d is waiting on even\n", x);
			sem_wait(&sem_even);		
		} else {	
			printf("Process %d is waiting on odd\n", x);
			sem_wait(&sem_odd);
		}	
	  } 
}


void processSlow ( void *ptr )
{
    int x,i;
    x = *((int *) ptr);
    i = 0;

    for(i; i<TOTAL_PHASES; i++) {
	sleep(3);
	printf("Process %d finished phase %d\n", x, i);
	// Phase finished, call synchronizing function
	barrierSynch(i,x);	
     }
    pthread_exit(0); /* exit thread */
}


void processFast ( void *ptr )
{
    int x,i;
    x = *((int *) ptr);
    i = 0;

    for(i; i<TOTAL_PHASES; i++) {
	printf("Process %d finished phase %d\n", x, i);
	// Phase finished, call synchronizing function
	barrierSynch(i,x);
     }
    /* exit thread */
    pthread_exit(0); 
}
