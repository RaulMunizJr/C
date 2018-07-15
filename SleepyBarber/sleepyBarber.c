
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "customer.c"
#include "shop.c"
#include "barber.c"

//useful variables
int occupied_chairs;
int MAX_CHAIRS;
int MIN_PROGRAMMING_TIME;
int MAX_PROGRAMMING_TIME;
int MIN_HELPING_TIME;
int MAX_HELPING_TIME;
int globalSeed;
int chairs[3];
int front;
int rear;

sem_t Barber_ready;
sem_t Barber_done;
sem_t Customer_register;

pthread_mutex_t setup_lock;
pthread_mutex_t printf_lock;
pthread_mutex_t rand_lock;
pthread_mutex_t chairs_lock;


void* Customer(void* param);
void* Barber(void* param);


int main(int argc, char** argv) 
{
	
    int number_of_customers;
    if (argc > 1)
        if (argc > 2)
        {    

            printf("Error! Check command line arguements!");
            return(EXIT_FAILURE);

        }

        else
        {
            number_of_customers = atoi(argv[1]);

            if (number_of_customers <1)
            {

                printf("Error! Check command line arguements!");
                return(EXIT_FAILURE);   

            }
        }

    else
    {

        number_of_customers = 5;
        
    }
    
    //golbal args and initializers for project
 	sem_init(&Barber_ready, 0,0);
    sem_init(&Barber_done, 0,0);
    sem_init(&Customer_register,0,0);
    

    MAX_CHAIRS = 3;
    occupied_chairs = 0;
    MIN_PROGRAMMING_TIME = 10;
    MAX_PROGRAMMING_TIME = 30;
    MIN_HELPING_TIME = 5;
    MAX_HELPING_TIME = 10;
    globalSeed= time(NULL);
    front = 0;
    rear = -1;

    pthread_mutex_init(&setup_lock, NULL);
    pthread_mutex_init(&printf_lock, NULL);
    pthread_mutex_init(&rand_lock, NULL);
    pthread_mutex_init(&chairs_lock, NULL);

   
    //lock mutex here
    pthread_t temp;

    pthread_mutex_lock(&setup_lock);
    pthread_create(&temp,NULL,Barber,NULL);
    pthread_mutex_lock(&setup_lock);

    int i;

    for (i = 1 ; i <= number_of_customers; i++)
    {

        pthread_create(&temp,NULL,Customer, &i);
        pthread_mutex_lock(&setup_lock);

    }
    pthread_mutex_unlock(&setup_lock);
    
    
    pthread_join(temp,NULL);
    
    return (EXIT_SUCCESS);
}
