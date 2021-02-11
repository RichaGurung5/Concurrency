// L5CG1, (unv\2052270) NP03A190200, AASHISH SUNUWAR ©2021

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//GLOBAL VARIABLES DECLARATION
pthread_mutex_t lock; //initializing mutex lock variable
double sum = 0;       //global resource (variable to store the sum value)

//DEFINING STRUCTURE FOR DIVIDING TASK UPON THREADS
struct
    iterables
{
    long long startingPoint, endingPoint; //variables to store the starting point and ending point of the iteration for each thread
};

//RECURSIVE FUNCTION TO RETURN THE VALUE OF -1 POWER TO THE n [(-1)^n]
double powerUp(long long b) //start of powerUp functions
{
    if (b == 0) //base case to end recurion
    {
        return 1;
    }

    return -1 * powerUp(b - 1); //recursion from exponentiation value to the base case

} //end of powerUp function

//FUNCTION TO CALCULATE SUM
void *findSum(void *arg) //start of findSum
{                        //function to be performed by threads
    struct
        iterables
            *s = (struct iterables *)arg;       //importing iteration data to perform iterations
    long long startingPoint = s->startingPoint; //assigning starting point for the thread from iteration data
    long long endingPoint = s->endingPoint;     //assigning ending point

    for (long long n = startingPoint; n <= endingPoint; n++) //looping from starting point to the ending point
    {
        pthread_mutex_lock(&lock);           //locking operation to use global resource
        sum += (powerUp(n)) / ((2 * n) + 1); //assigning calculated value to the global resource i.e. sum
        pthread_mutex_unlock(&lock);         //unlocking operation for other threads
    }
    pthread_exit(0); //exiting from the thread returning void(null) value

} //end of findSum function

//FUNCTION TO SLICE THE ITERATIONS EQUALLY FOR EACH THREAD
struct iterables *sliceData(int noOfThreads, long long iterations) //start of sliceData function
{
    int slicedList[noOfThreads];              //creating sliceList equals to number of threads given
    int remainder = iterations % noOfThreads; //calculating remainings after sharing the iteration

    for (int i = 0; i < noOfThreads; i++) //for every thread
    {
        slicedList[i] = iterations / noOfThreads; //slice data equally
    }

    for (int i = 0; i < remainder; i++) //for every remainings
    {
        slicedList[i] = slicedList[i] + 1; //share it equally between first few threads
    }

    int startingPointList[noOfThreads]; //starting point for every thread
    int endingPointList[noOfThreads];   //ending point for every thread

    for (int i = 0; i < noOfThreads; i++) //for each thread
    {
        if (i == 0) //if it is first iteration
        {
            startingPointList[i] = 0; //assigning first iteration as the starting point for first thread
        }
        else // if it isn't the first iteration
        {
            startingPointList[i] = endingPointList[i - 1] + 1; //assgning the starting point next to the ending point of previous thread i.e.ending_point_of_previous_thread + 1
        }
        endingPointList[i] = startingPointList[i] + slicedList[i] - 1; //divided value (sliced data) added to its starting point is its ending point
    }

    //STRUCTURE TO STORE ITERATION STARTING AND ENDING POINT
    struct
        iterables
            *structure = malloc(noOfThreads * sizeof(struct iterables)); //dynamically allocating memory according to our number of threads

    for (int i = 0; i < noOfThreads; i++) //for every thread
    {
        structure[i].startingPoint = startingPointList[i]; //store its starting point to its structure
        structure[i].endingPoint = endingPointList[i];     //store its ending point to its structure
    }

    return structure; //returning the value assigned structures

} //end of sliceData function

void main() //startof main function
{
    //VARIABLES DECLARATION
    long long iterations;
    int noOfThreads;

    //ASKING NUMBER OF ITERATIONS INPUT TO PERFORM WITH THE USER
    printf("Enter number of iterations to perform: ");
    scanf("%lld", &iterations);

    //ASKING NUMBER OF THREADS THEY WANT TO USE
    printf("Enter number of threads you want to use: ");
    scanf("%d", &noOfThreads);

    struct iterables *structure = sliceData(noOfThreads, iterations); //importing the value assigned structures to the new structure

    pthread_t thread[noOfThreads]; //threads declaration

    //CREATING THREADS
    for (int i = 0; i < noOfThreads; i++)
    {
        pthread_create(&thread[i], NULL, findSum, &structure[i]);
    }

    //WAINTING FOR THREADS TO RUN COMPLETELY
    for (int i = 0; i < noOfThreads; i++)
    {
        pthread_join(thread[i], NULL);
    }

    pthread_mutex_destroy(&lock); //destroying the mutex lock variable after every threads ran completely

    //PRINTING OUT THE CALCULATED PI VALUE
    printf("\nValue of Pi according to Leibniz formula is %lf.", 4 * sum);

} //end of main function