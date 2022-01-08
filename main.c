#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#include "src/crs2/calculation.h"
#include "src/points/point.h"
#include "src/tests/test_functions.h"


double generateValue(int argNo, constraint_function_t checkConstraints) {
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

    // function which minimum is needed
    evaluated_function_t evaluatedFunction         = NULL;
    constraint_function_t checkConstraintsFunction = NULL;

    //TODO: add selection
    evaluatedFunction        = goldstein_price_function;
    checkConstraintsFunction = goldstein_price_constraint;

    point_t** points = calloc(N, sizeof(point_t*));
    for(int i = 0; i < N; i++) {
        point_t *point = NULL;
        point = Point_Init(point, n);

        for(int j = 0; j < n; j++) {
            double v = generateValue(j, checkConstraintsFunction);
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
        solution1 = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraintsFunction, Parallel);
        end = clock();
        timeParallel += (double)(end - start) / (double)(CLOCKS_PER_SEC);

        start = clock();
        solution2 = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraintsFunction, Sequential);
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
