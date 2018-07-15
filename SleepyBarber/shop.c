
#include <pthread.h>
#include <stdlib.h>

//useful variables
extern int MIN_PROGRAMMING_TIME;
extern int MAX_PROGRAMMING_TIME;
extern int MIN_HELPING_TIME;
extern int MAX_HELPING_TIME;

extern pthread_mutex_t rand_lock;

//Barber seed HELPING_TIME 
//Function to generate a random number from a seed pointer
//Customer seed PROGRAMMING_TIME 
int generateSomeTime(int caller_type, unsigned int* seed)
{
    if (caller_type)
    { 
        
        pthread_mutex_lock(&rand_lock);
        int period = rand_r(seed);	//random!!
        pthread_mutex_unlock(&rand_lock);

        
        return (MIN_PROGRAMMING_TIME+period%(MAX_PROGRAMMING_TIME-MIN_PROGRAMMING_TIME));
    }

    else
    {


        pthread_mutex_lock(&rand_lock);
        int period = rand_r(seed); //random!
        pthread_mutex_unlock(&rand_lock);
        

        return (MIN_HELPING_TIME+period%(MAX_HELPING_TIME-MIN_HELPING_TIME));
    }
}
