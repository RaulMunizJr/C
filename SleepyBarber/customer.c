
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


//*****************Customer function********************************
void* Customer(void* param)
{
    unsigned int seed = globalSeed;
    globalSeed++;

    int customerID = *((int*) param);
    printf("Customer:%d called!\n", customerID);

    pthread_mutex_unlock(&setup_lock);
    while(1)
    {
        int time = generateSomeTime(1, &seed);	
        printf("Customer:%d will wait %d seconds.\n", customerID, time);
        sleep(time);	
        printf("Customer:%d inteds to go to Barber\n", customerID);	//Customer done with waiting
        
       
        pthread_mutex_lock(&chairs_lock);
        if (occupied_chairs<MAX_CHAIRS)	//Check for chairs
        {
            occupied_chairs++;	
            rear++;
            rear = rear % MAX_CHAIRS;
            chairs[rear] = customerID;


            printf("Customer:%d waiting for Barber...\n", customerID);
			sem_post(&Customer_register);	//Inform Barber that customer is waiting
            pthread_mutex_unlock(&chairs_lock);	


            sem_wait(&Barber_ready);	//Wait till Barber is done with previous customer
			sem_wait(&Barber_done);	//Wait till Barber is done with current customer     

        }
        else
        {

            pthread_mutex_unlock(&chairs_lock);	//no chair
            printf("Customer:%d did not find chairs!\n", customerID);

        }
    }
    

    pthread_exit(0);
}
