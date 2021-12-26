#ifndef CALCULATION_SEQUENTIAL_H
#define CALCULATION_SEQUENTIAL_H

#include "calculation.h"

struct point_t* CalculationSequential_findMinimum(struct point_t **A, const int n, const int N, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction);

#endif // CALCULATION_SEQUENTIAL_H
