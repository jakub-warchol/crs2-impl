#define CVECTOR_LOGARITHMIC_GROWTH

#include "calculation.h"
#include "../points/point.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>


static point_t* findMinimum(point_t **A, const int N, const int n, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction) {
    // init some used below variables
    point_t* G = NULL;
    G = Point_Init(G, n);

    point_t* P = NULL;
    P = Point_Init(P, n);

    point_t *M = NULL;
    point_t *L = NULL;
    point_t *R = calloc(n + 1, sizeof(point_t));


    while(true) {
        // find best and worst function
        M = A[0];
        L = A[0];

        for(int i = 0; i < N; i++) {
            point_t *it = A[i];
            double fValue = it->value;
            if(fValue < L->value) {
                L = it;
            }

            if(fValue > M->value) {
                M = it;
            }
        }

        while(true) {
            // create R's symplex
            R[0] = *L;

            for(int i = 1; i < n + 1;) {
                int idx = rand() % N;
                if(A[idx] != L) { // if chosen point is diffrent than the best
                    R[i] = *A[idx]; // add to symplex
                    i++;
                }
            }

            // find centroid
            for(int i = 0; i < n; i++) {
                point_t *p = &R[i];
                for(int j = 0; j < n; j++) {
                    G->args[j] += p->args[j];
                }
            }

            for(int i = 0; i < G->argCount; i++) {
                G->args[i] /= G->argCount;

                // determine next trial point
                P->args[i] = 2 * G->args[i] - R[n].args[i];
            }

            // check constrains
            bool constraintPassed = true;
            for(int i = 0; i < P->argCount; i++) {
                if(constaintFunction(i, P->args[i]) != 0) {
                    constraintPassed = false;
                    break;
                }
            }

            if(!constraintPassed) {
                continue;
            }

            // evaluate value for P
            double valueP = evaluatedFunction(P->argCount, P->args);
            P->value = valueP;

            // if f_p < f_m, swap points
            if(P->value < M->value) {
                Point_Swap(P, M);
                break;
            }
        }

        // check stop criterion
        if((M->value / L->value) < 1.001) {
            break; // congartulations, you've found the minimum!
        }
    }

    // create solution point
    point_t *solution = NULL;
    solution = Point_Init(solution, n);
    memcpy(solution, L, sizeof(point_t));

    // remove G and P
    Point_Destroy(G);
    Point_Destroy(P);

    // free memory
    free(R);

    return solution;
}

point_t *Calculation_FindMinimum(point_t **points, const int n, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction, execution_type_t executionType)
{
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
        solution = findMinimum(points, N, n, evaluatedFunction, constaintFunction);
    } else if(executionType == Parallel) { // run in multiple thread

    } else if(executionType == Distribution) { // run in gRPC/MPI

    }

    return solution;
}
