#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#include "src/crs2/calculation.h"
#include "src/points/point.h"

#define M_PI 3.14159265358979323846

/// TODO: temporary macro
#define f(x1, x2) (1 + pow((x1 + x2 + 1),2) * (19 - 14*x1 + 3*pow(x1,2) - 14*x2 + 6*x1*x2 + 3*pow(x2,2))) * (30 + pow((2*x1 - 3*x2),2) * (18 - 32*x1 + 12*pow(x1,2) + 48*x2 - 36*x1*x2 + 27*pow(x2,2))) //GOLDSTEIN-PRICE FUNCTION

//#define f(x1, x2) pow(sin(3*M_PI*x1),2) + pow((x1-1),2) * pow(1+(sin(3*M_PI*x2)),2) + pow((x2-1),2) * pow(1+(sin(2*M_PI*x2)),2) //LEVY FUNCTION N. 13

//#define f(x1, x2) -(x2+47) * sin(sqrt(abs(x2+x1/2+47))) - x1 * sin(sqrt(abs(x1-(x2+47)))) //EGGHOLDER FUNCTION

//#define f(x1, x2) -(1 + cos(12*sqrt(pow(x1,2)+pow(x2,2)))) / (0.5*(pow(x1,2)+pow(x2,2)) + 2); //DROP-WAVE FUNCTION

//#define f(x1, x2) sin(x1 + x2) + pow((x1 - x2),2) - (1.5*x1) + 2.5*x2 + 1 //MCCORMICK FUNCTION

double evaluatedFunction(const int n, double *args) {
    return f(args[0], args[1]);
}

int checkConstraints(int argNo, double value) {
    if(argNo == 0) {
        if(value >= -2) {
            return 0;
        } else {
            return -1;
        }
    } else if(argNo == 1) {
        if(value <= 2) {
            return 0;
        } else {
            return 1;
        }
    }

    return -1;
}

double generateValue(int argNo) {
    int sign = (rand() % 2 == 0) ? 1 : -1;
    double mul = 10;
    double value = sign * ((double)rand() / (double)RAND_MAX) * mul;

    while(true) {
        int res = checkConstraints(argNo, value);
        if(res == 0) {
            break;
        } else if(res == 1) {
            mul /= 2;
        } else {
            mul *= 2;
        }

        sign  = (rand() % 2 == 0) ? 1 : -1;
        value = sign * ((double)rand() / (double)RAND_MAX) * mul;
    }

    return value;
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    const int n = 2; // no of variables
    const int N = 10 * (n + 1);

    point_t** points = calloc(N, sizeof(point_t*));
    for(int i = 0; i < N; i++) {
        point_t *point = NULL;
        point = Point_Init(point, n);

        for(int j = 0; j < n; j++) {
            double v = generateValue(j);
            point->args[j] = v;
        }

        points[i] = point;
    }

    printf("Found minimum: ");
    fflush(stdout);

    clock_t start, end;
    double timeParallel = 0,timeSequential = 0;
    point_t *solution1, *solution2;

    int testInterations = 10;

    for (int i = 0 ; i<testInterations; i++){

        start = clock();
        solution1 = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraints, Parallel);
        end = clock();
        timeParallel += (double)(end - start) / (double)(CLOCKS_PER_SEC);

        start = clock();
        solution2 = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraints, Sequential);
        end = clock();
        timeSequential += (double)(end - start) / (double)(CLOCKS_PER_SEC);

    }

    Point_Print(solution1);
    Point_Print(solution2);

    printf("parallel: %f \n",timeParallel/testInterations);
    printf("sequential: %f \n",timeSequential/testInterations);

    for(int i = 0; i < N; i++) {
        Point_Destroy(points[i]);
    }
    free(points);

    return 0;
}
