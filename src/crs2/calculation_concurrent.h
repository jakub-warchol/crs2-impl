#ifndef CALCULATION_CONCURRENT_H
#define CALCULATION_CONCURRENT_H

#include <stdbool.h>

#include "calculation.h"

typedef struct concurrent_data_t{
    // init data
    struct point_t **A;
    int N;
    int n;
    evaluated_function_t evaluatedFunction;
    constraint_function_t constaintFunction;

    // synchronization data
    volatile bool solutionFound;
    struct point_t *solution;
} concurrent_data_t;

struct point_t* CalculationConcurrent_findMinimum(struct point_t **points, const int n, const int N, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction);

#endif // CALCULATION_CONCURRENT_H
