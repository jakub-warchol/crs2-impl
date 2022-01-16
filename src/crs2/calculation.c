#include "calculation.h"
#include "calculation_sequential.h"
#include "calculation_concurrent.h"
#include "calculation_distributed.h"
#include "../points/point.h"

#include <stdlib.h>
#include <assert.h>

point_t *Calculation_FindMinimum(point_t **points, const int n, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction, execution_type_t executionType)
{
    assert(points != NULL && "Given points are null");
    assert(evaluatedFunction != NULL && "Evaluated function is null");
    assert(constaintFunction != NULL && "Constraint function is null");

    const int N = 10 * (n + 1);
    point_t *solution = NULL;

    // generate A's array
    for(int i = 0; i < N; i++) {
        point_t *point = points[i];

        // evaluate function
        double funValue = evaluatedFunction(point->argCount, point->args);
        point->value    = funValue;
    }

    if(executionType == Sequential) { // run in one thread
        solution = CalculationSequential_findMinimum(points, n, N, evaluatedFunction, constaintFunction);
    } else if(executionType == Parallel) { // run in multiple thread
        solution = CalculationConcurrent_findMinimum(points, n, N, evaluatedFunction, constaintFunction);
    } else if(executionType == Distribution) { // run in gRPC/MPI
        solution = CalculationDistributed_findMinimum(points, n, N, evaluatedFunction, constaintFunction);
    }

    return solution;
}
