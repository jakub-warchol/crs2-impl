#ifndef CALCULATION_DISTRIBUTED_H
#define CALCULATION_DISTRIBUTED_H

#include "calculation.h"

struct point_t* CalculationDistributed_findMinimum(struct point_t **points, const int n, const int N, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction);

#endif // CALCULATION_DISTRIBUTED_H
