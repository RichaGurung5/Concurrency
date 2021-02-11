// L5CG1, (unv\2052270) NP03A190200, AASHISH SUNUWAR ©2021

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//GLOBAL VARIABLES DECLARATION
pthread_mutex_t lock;                     //initializing mutex lock variable
int *data, *primeNumbers, primeCount = 0; //global resource (variable to store prime numbers and count of them)

//DEFINING STRUCTURE FOR DIVIDING TASK UPON THREADS
struct
    iterables
{
    long long startingPoint, endingPoint; //variables to store the starting point and ending point of the iteration for each thread
};

//FUNCTION TO COUNT PRIME NUMBERS
void *
countPrime(void *arg) //start of countPrime function
{
    struct
        iterables
            *s = (struct iterables *)arg;       //importing iteration data to perform iterations
    long long startingPoint = s->startingPoint; //assigning starting point for the thread from iteration data
    long long endingPoint = s->endingPoint;     //assigning ending point
    // int prime;

    for (long long n = startingPoint; n <= endingPoint; n++) //looping from starting point to the ending point
    {
        int check = 1;                    //setting current number as prime number
        if (data[n] == 0 || data[n] == 1) //if x is 0 or 1
        {
            check = 0; //current number is not prime number
        }
        for (int i = 2; i < data[n]; i++) //for numbers from 2 to the current number
        {
            if (data[n] % i == 0) //if current number is completely divisible by any number up to it
            {
                check = 0; //current number is not prime number
                break;
            }
        }
        if (check == 1) //if current number is still prime number then
        {
            pthread_mutex_lock(&lock);          //locking operation to use global resource
            primeNumbers[primeCount] = data[n]; //adding the current number in prime numbers list
            primeCount++;                       //increasing count of prime numbers by 1
            pthread_mutex_unlock(&lock);        //unlocking operation for other threads
        }
    }
    pthread_exit(0); //exiting from the thread returning void(null) value

} //end of countPrime function

//FUNCTION TO ALLOCATE MEMORY DYNAMICALLY
void memoryAllocation(int noOfData) //start of memoryAllocation function
{
    //ALLOCATING MEMORY TO STORE DATA FROM FILE
    data = malloc(noOfData * sizeof(int));
    if (data == NULL)
    {
        printf("Couldn't allocate memory. Insufficient storage.");
        exit(0);
    }

    //ALLOCATING MEMORY TO STORE PRIME NUMBERS
    primeNumbers = malloc(noOfData * sizeof(int));
    if (primeNumbers == NULL)
    {
        printf("Couldn't allocate memory. Insufficient storage.");
        exit(0);
    }

} //start of memoryAllocation function

// FUNCTION TO COUNT TOTAL NUMBER OF DATA
int dataCounter() //start of dataCounter function
{
    int noOfData = 0, temp;

    FILE *fp[3];                          //opening files
    fp[0] = fopen("PrimeData1.txt", "r"); //first file
    fp[1] = fopen("PrimeData2.txt", "r"); //second file
    fp[2] = fopen("PrimeData3.txt", "r"); //third file

    for (int i = 0; i < 3; i++) //for each file
    {
        while (fscanf(fp[i], "%d", &temp) != EOF) //as long as it reaches at the End Of File (EOF)
        {
            noOfData++; //count number of data available
        }
        fclose(fp[i]); //close files
    }
    return noOfData; //return total data found

} //end of dataCounter function

//FUNCTION TO SLICE THE ITERATIONS EQUALLY FOR EACH THREAD
struct iterables *sliceData(int noOfThreads, int noOfData) //start of sliceData function
{
    int slicedList[noOfThreads];            //creating sliceList equals to number of threads given
    int remainder = noOfData % noOfThreads; //calculating remainings after sharing the iteration

    for (int i = 0; i < noOfThreads; i++) //for every thread
    {
        slicedList[i] = noOfData / noOfThreads; //slice data equally
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
            *structures = malloc(noOfThreads * sizeof(struct iterables)); //dynamically allocating memory according to our number of threads
    for (int i = 0; i < noOfThreads; i++)                                 //for every thread
    {
        structures[i].startingPoint = startingPointList[i]; //store its starting point to its structure
        structures[i].endingPoint = endingPointList[i];     //store its ending point to its structure
    }

    return structures; //returning the value assigned structures

} //end of sliceData function

//FUNCTION TO READ FILES AND IMPORT DATA
void readFiles() //start of readFiles function
{
    int j = 0;
    FILE *fp[3];                          //reading 3 files
    fp[0] = fopen("PrimeData1.txt", "r"); //file1
    fp[1] = fopen("PrimeData2.txt", "r"); //file2
    fp[2] = fopen("PrimeData3.txt", "r"); //file3

    for (int i = 0; i < 3; i++) // for each file
    {
        while (fscanf(fp[i], "%d", &data[j]) != EOF) //scan every data and store it to data list
        {
            j++;
        }
        fclose(fp[i]); //closing each file
    }

} //end of readFiles function

// FUNCTION TO WRITE PRIME NUMBERS INTO A FILE
void writeOnFile() //start of writeOnFile function
{
    if (primeCount > 0) //if prime numbers list is not empty
    {
        FILE *p = fopen("primenumbers.txt", "w");               //create a file in OutputFiles folder
        fprintf(p, "Total prime numbers: %d.\n\n", primeCount); //write on total prime numbers found
        for (int i = 0; i < primeCount; i++)                    //for every prime number
        {
            fprintf(p, "%d\n", primeNumbers[i]); //write it into file
        }

        fclose(p); //close that file

        //ASKING USER TO CHECK OUTPUT FILE
        printf("Check 'primenumbers.txt' file to see all the prime numbers found.");
    }

} //end of writeOnFile function

void main() //start of main function
{
    //VARIABLES DECLARATION
    int noOfThreads, noOfData;

    //ASKING USER FOR NUMBER OF THREADS THEY WANT TO USE
    printf("Enter number of threads you want to use: ");
    scanf("%d", &noOfThreads);

    noOfData = dataCounter(); //getting total number of data

    memoryAllocation(noOfData); //allocating memory according to the total number of data

    readFiles(); //reading (importing) data

    struct iterables *structures = sliceData(noOfThreads, noOfData); //importing the value assigned structures to the new structure

    pthread_t thread[noOfThreads]; //threads declaration

    //CREATING THREADS
    for (int i = 0; i < noOfThreads; i++)
    {
        pthread_create(&thread[i], NULL, countPrime, &structures[i]);
    }

    //WAINTING FOR THREADS TO RUN COMPLETELY
    for (int i = 0; i < noOfThreads; i++)
    {
        pthread_join(thread[i], NULL);
    }
    pthread_mutex_destroy(&lock); //destroying the mutex lock variable after every threads ran completely

    // After calculation
    printf("\n%d total prime numbers found.\n", primeCount);

    // writing prime numbers found in a seperate file
    writeOnFile();

    // Free up dynamically allocated memory
    free(data);
    free(primeNumbers);

} //end of main function