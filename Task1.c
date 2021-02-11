// L5CG1, (unv\2052270) NP03A190200, AASHISH SUNUWAR ©2021

#include <stdio.h>

void main() //main function start
{
    //IMPORTANT VARIABLES DECLARATION
    int n = 0;
    double a, b, input, prediction, x[500], y[500], sumX = 0, sumY = 0, sumX2 = 0, sumY2 = 0, sumXY = 0;

    //OPENING FILES
    FILE *fp[4];                          //creating 4 file pointers to read 4 files
    fp[0] = fopen("datasetLR1.txt", "r"); //first file
    fp[1] = fopen("datasetLR2.txt", "r"); //second file
    fp[2] = fopen("datasetLR3.txt", "r"); //third file
    fp[3] = fopen("datasetLR4.txt", "r"); //fourth file

    //READING FILES
    for (int i = 0; i < 4; i++) //for every file
    {
        while (fscanf(fp[i], "%lf,%lf", &x[n], &y[n]) != EOF) //scan datas and pass it into x and y respectively until we reach to the end of the file (EOF)
        {
            n++; //keep count of the total numbers of data
        }
        fclose(fp[i]); //close file after reaching EOF
    }

    //CALCULATION OF REQUIRED VALUES
    for (int i = 0; i < n; i++) //for each data
    {
        sumX += x[i];         //taking sum of every x (Total x)
        sumY += y[i];         //taking sum of every y (Total y)
        sumX2 += x[i] * x[i]; //taking sum of every xSquare (Total x*x)
        sumY2 += y[i] * y[i]; //taking sum of every ySquare (Total y*y)
        sumXY += x[i] * y[i]; //taking sum of every x+y (Total x+y)
    }

    //IMPLEMENTING GIVEN FORMULA TO FIND OUT SLOPE AND LINE-INTERCEPT OF OUR BEST FIT LINE
    a = ((sumY * sumX2) - (sumX * sumXY)) / ((n * sumX2) - (sumX * sumX)); //line-intercept
    b = ((n * sumXY) - (sumX * sumY)) / ((n * sumX2) - (sumX * sumX));     //slope

    //PRINTING OUT THE CALCULATED SLOPE AND INTERCEPT i.e. a && b
    printf("The equation for the best-fit line is y = %lf * x + %lf. \n", b, a);

    //ASKING FOR THE INPUT TO THE USER
    printf("Enter value of x: ");
    scanf("%lf", &input);

    //PREDICTING VALUE ACCORDING TO THE GIVEN INPUT
    prediction = b * input + a;

    //PRINTING OUT THE PREDICTION
    printf("\nPredicted value of y for %lf is %lf.", input, prediction);

} //end of main function