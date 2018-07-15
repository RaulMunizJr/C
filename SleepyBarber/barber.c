
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


//useful variables
extern int setupNotDone;
extern int globalSeed;
extern int occupied_chairs;
extern int MAX_CHAIRS;
extern int chairs[];
extern int front;
extern int rear;

extern sem_t Barber_ready;
extern sem_t Barber_done;
extern sem_t Customer_register;

extern pthread_mutex_t printf_lock;
extern pthread_mutex_t setup_lock;
extern pthread_mutex_t chairs_lock;
int generateSomeTime(int, unsigned int*);


//*************************Barber function*****************************************
void* Barber(void* param)
{
    unsigned int seed = globalSeed;
    globalSeed++;

    printf("Barber has been called!\n");
    pthread_mutex_unlock(&setup_lock); 


    while(1)
    {
        pthread_mutex_lock(&chairs_lock);	

		if (occupied_chairs == 0)
	        printf("No Customers waiting, Barber will sleep!\n");


        pthread_mutex_unlock(&chairs_lock);	
        sem_wait(&Customer_register);	
        pthread_mutex_lock(&chairs_lock);	
        occupied_chairs--;	


        int customerID = chairs[front];	
        front++;
        front = front%MAX_CHAIRS;


        int time = generateSomeTime(0, &seed);	
        printf("Barber will attend Customer:%d for %d seconds.\n", customerID , time);


        pthread_mutex_unlock(&chairs_lock);	
        sleep(time);	
	    printf("Customer:%d done with Barber!\n", customerID);


        sem_post(&Barber_done);	
        sem_post(&Barber_ready);
    }
    
    
    pthread_exit(0);
}
