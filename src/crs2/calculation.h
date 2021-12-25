#ifndef CALCULATION_H
#define CALCULATION_H

struct point_t;

typedef double (*evaluated_function_t)(const int argCount, double *args);
typedef int (*constraint_function_t)(int argNo, double value);


typedef enum {
    Sequential,
    Parallel,
    Distribution,
} execution_type_t;


struct point_t* Calculation_FindMinimum(struct point_t **points, const int n, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction, execution_type_t executionType);

#endif // CALCULATION_H
