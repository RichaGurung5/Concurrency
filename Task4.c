// L5CG1, (unv\2052270) NP03A190200, AASHISH SUNUWAR ©2021

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "lodepng.h"

//GLOBAL VARIABLES TO STORE THE PIXEL DATAS
int givenImagePixelValues[20000][20000], blurredImagePixelValues[20000][20000];

//DEFINING STRUCTURE FOR DIVIDING TASK UPON THREADS
struct iterables
{
    long long startingPoint, endingPoint; //variables to store the starting point and ending point of the iteration for each thread
    long long width, height;              //variables to store the actual width and height of the image
};

//FUNCTION TO APPLY GAUSSIAN BLUR
void *gaussianBlur(void *arg) //start of gaussianBlur function
{
    struct
        iterables
            *s = (struct iterables *)arg;       //importing iteration data to perform iterations
    long long startingPoint = s->startingPoint; //assigning starting point for the thread from iteration data
    long long endingPoint = s->endingPoint;     //assigning ending point
    long long height = s->height;               //assigning height of the image
    long long width = s->width;                 //assigning width of the image

    for (long long n = startingPoint; n <= endingPoint; n++) //looping from starting point to the ending point
    {
        long long column = n, row = 0, sumR = 0, sumG = 0, sumB = 0;

        while (column >= width)
        {
            column -= width;
            row++;
        }

        //TAKING 3X3 KERNEL FOR APPLYING GAUSSIAN BLUR FILTER
        /*  

            X . .
            . O .
            . . #  
        
            To apply gaussian blur filter in O, we start loop from X i.e.(R-1,C-1) from O and end in # i.e.(R+2,C+2) from the starting point(X)
        */
        for (int i = row - 1; i < row + 2; i++)
        {
            for (int j = column - 1; j < column + 2; j++) //up to
            {
                if (i > height || i < 0 || j > width || j < 0) //if the indexing doesn't lie on the image
                {
                    continue; //ignore
                }
                else //but if it does, add up its RGB values to the respective variables
                {
                    sumR += givenImagePixelValues[i][4 * j + 0]; //sum of red values of pixels inside the kernel
                    sumG += givenImagePixelValues[i][4 * j + 1]; //sum of green values of pixels inside the kernel
                    sumB += givenImagePixelValues[i][4 * j + 2]; //sum of blue values of pixels inside the kernel
                }
            }
        }
        //IF IT IS CORNER PIXELS
        if ((row == 0 && column == 0) || (row == 0 && column == width - 1) || (row == height - 1 && column == 0) || (row == height - 1 && column == width - 1))
        {
            //DIVIDING ITS SUM BY 4 AS IT WILL HAVE ONLY 4 PIXELS IN 3X3 KERNEL
            blurredImagePixelValues[row][4 * column + 0] = sumR / 4;                                   //assigning average of the kernel to red
            blurredImagePixelValues[row][4 * column + 1] = sumG / 4;                                   //assigning average of the kernel to green
            blurredImagePixelValues[row][4 * column + 2] = sumB / 4;                                   //assigning average of the kernel to blue
            blurredImagePixelValues[row][4 * column + 3] = givenImagePixelValues[row][4 * column + 3]; //no need to manipulate transparency values so passing same as the original
        }
        //IF IT IS SIDE PIXELS
        else if ((row == 0 && column < width - 1) || (row < height - 1 && column == width - 1) || (row == height - 1 && column < width - 1) || (row < height - 1 && column == 0))
        {
            //DIVIDING ITS SUM BY 4 AS IT WILL HAVE ONLY 6 PIXELS IN 3X3 KERNEL
            blurredImagePixelValues[row][4 * column + 0] = sumR / 6;                                   //assigning average of the kernel to red
            blurredImagePixelValues[row][4 * column + 1] = sumG / 6;                                   //assigning average of the kernel to green
            blurredImagePixelValues[row][4 * column + 2] = sumB / 6;                                   //assigning average of the kernel to blue
            blurredImagePixelValues[row][4 * column + 3] = givenImagePixelValues[row][4 * column + 3]; //no need to manipulate transparency values so passing same as the original
        }
        //OTHER PIXELS
        else
        {
            //DIVIDING ITS SUM BY 9 AS IT WILL HAVE ALL OF THE PIXELS IN 3X3 KERNEL
            blurredImagePixelValues[row][4 * column + 0] = sumR / 9;                                   //assigning average of the kernel to red
            blurredImagePixelValues[row][4 * column + 1] = sumG / 9;                                   //assigning average of the kernel to green
            blurredImagePixelValues[row][4 * column + 2] = sumB / 9;                                   //assigning average of the kernel to blue
            blurredImagePixelValues[row][4 * column + 3] = givenImagePixelValues[row][4 * column + 3]; //no need to manipulate transparency values so passing same as the original
        }
    }
    pthread_exit(0); //exiting from the thread returning void(null) value
}

//FUNCTION TO SLICE THE ITERATIONS EQUALLY FOR EACH THREAD
struct iterables *sliceData(int noOfThreads, long long totalPixel, unsigned int width, unsigned int height) //start of sliceData function
{
    int slicedList[noOfThreads];              //creating sliceList equals to number of threads given
    int remainder = totalPixel % noOfThreads; //calculating remainings after sharing the iteration

    for (int i = 0; i < noOfThreads; i++) //for every thread
    {
        slicedList[i] = totalPixel / noOfThreads; //slice data equally
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
        structures[i].width = width;
        structures[i].height = height;
    }

    return structures; //returning the value assigned structures

} //end of sliceData function

void main() //start of main function
{
    //VARIABLES DECLARATION
    int noOfThreads;
    unsigned error;
    unsigned char *image;
    unsigned int width, height;
    char filename[100];

    //ASKING USER FOR NUMBER OF THREADS THEY WANT TO USE
    printf("Enter number of threads you want to use: ");
    scanf("%d", &noOfThreads);

    //ASKING USER FOR PNG IMAGE FILE
    printf("Enter the input file name: \n");
    scanf("%s", filename);

    //DECODING THE IMAGE PROVIDED BY THE USER
    error = lodepng_decode32_file(&image, &width, &height, filename); //store the properties in image,width and height variables
    if (error)                                                        //if any error occurs while decoding the image
    {
        printf("Error while decoding image %u: %s", error, lodepng_error_text(error)); //provide message to the user about the error
    }

    //STORING THE RGBA VALUES OF GIVEN IMAGE PIXELS FOR THE MANIPULATION
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            givenImagePixelValues[i][4 * j + 0] = image[4 * width * i + 4 * j + 0]; //r value of pixel
            givenImagePixelValues[i][4 * j + 1] = image[4 * width * i + 4 * j + 1]; //g value of pixel
            givenImagePixelValues[i][4 * j + 2] = image[4 * width * i + 4 * j + 2]; //b value of pixel
            givenImagePixelValues[i][4 * j + 3] = image[4 * width * i + 4 * j + 3]; //a value of pixel
        }
    }

    long long totalPixel = width * height; //calculating total no of pixels in the given image

    struct iterables *structures = sliceData(noOfThreads, totalPixel, width, height); //slicing total pixel to each thread for manipulation

    pthread_t *thread = malloc(noOfThreads * sizeof(pthread_t)); //dynamically allocating memory to create threads according to the amount of the threads provided by the user

    //CREATING THREADS
    for (int i = 0; i < noOfThreads; i++)
    {
        pthread_create(&thread[i], NULL, gaussianBlur, &structures[i]);
    }

    //WAITING FOR THE THREADS TO RUN COMPLETELY
    for (int i = 0; i < noOfThreads; i++)
    {
        pthread_join(thread[i], NULL);
    }

    size_t pngsize;

    //STORING THE MANIPULATED VALUES OF PIXELS INTO THE image VARIABLE TO FURTHER ENCODE INTO AN IMAGE
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[4 * width * i + 4 * j + 0] = blurredImagePixelValues[i][4 * j + 0]; //blurred r value of a pixel
            image[4 * width * i + 4 * j + 1] = blurredImagePixelValues[i][4 * j + 1]; //blurred g value of a pixel
            image[4 * width * i + 4 * j + 2] = blurredImagePixelValues[i][4 * j + 2]; //blurred b value of a pixel
            image[4 * width * i + 4 * j + 3] = blurredImagePixelValues[i][4 * j + 3]; //blurred a value of a pixel
        }
    }

    //ENCODING THE MANIPULATED PIXEL VALUES INTO IMAGE
    error = lodepng_encode32(&image, &pngsize, image, width, height); //encoding an image
    if (error)                                                        //if any error occurs while encoding the image
    {
        printf("Error while encoding image %u: %s", error, lodepng_error_text(error)); //printing the actual error to the user
    }
    else //if there is no error while encoding
    {
        lodepng_save_file(image, pngsize, "blurred.png"); //saving the encoded image
        printf("Check 'blurred.png' for blurred image."); //outputing the filename of the encoded image
    }
    free(image); //free up image memory

} //end of main function