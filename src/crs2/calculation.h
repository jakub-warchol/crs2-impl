#ifndef CALCULATION_H
#define CALCULATION_H

#define CLC_RESOLUTION 1.001
#define CLC_MAX_ITERATIONS 10000

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
