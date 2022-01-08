#include "calculation_concurrent.h"
#include "../points/point.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 4

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void* findMinimum(point_t **A, const int N, const int n, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction, volatile bool *solutionFound, point_t *finalSolution) {
    // init some used below variables
    point_t* G = NULL;
    G = Point_Init(G, n);

    point_t* P = NULL;
    P = Point_Init(P, n);

    point_t *M = NULL;
    point_t *L = NULL;
    point_t *R = calloc(n + 1, sizeof(point_t));

    int counter = 0;

    while(true) {
        // find best and worst function
        M = A[0];
        L = A[0];

        // mutex
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < N; i++) {

            point_t *it = A[i];
            double fValue = it->value;

            if(fValue < L->value) {
                L = it;
            }else if(fValue > M->value) {
                M = it;
            }
        }
        pthread_mutex_unlock(&mutex);

        while(true) {
            // create R's simplex
            pthread_mutex_lock(&mutex);
            R[0] = *L;

            for(int i = 1; i < n + 1;) {
                int idx = rand() % N;

                if(A[idx] != L) { // if chosen point is diffrent than the best
                    R[i] = *A[idx]; // add to simplex
                    i++;
                }
            }
            pthread_mutex_unlock(&mutex);

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

            // if solution was already found, just break loop
            if(*solutionFound == true) {
                break;
            }

            // mutex
            pthread_mutex_lock(&mutex);

            // if f_p < f_m, swap points
            if(P->value < M->value) {
                Point_Swap(P, M);
                pthread_mutex_unlock(&mutex);

                break;
            }

            pthread_mutex_unlock(&mutex);
        }

        counter++;

        // check stop criterion
        if((M->value / L->value) < CLC_RESOLUTION || *solutionFound == true) {
            break; // congratulations, you've found the minimum!
        }else if (counter > CLC_MAX_ITERATIONS){
            printf("Max interations!\n");
            break;
        }

    }

    // create solution point
    if(*solutionFound == false) {
        *solutionFound = true;

        pthread_mutex_lock(&mutex);
        *finalSolution = *L;
        pthread_mutex_unlock(&mutex);
    }

    // remove G and P
    Point_Destroy(G);
    Point_Destroy(P);

    // free memory
    free(R);

    return NULL;
}

static void* runCalculation(void *args) {
    concurrent_data_t *data = (concurrent_data_t*)args;
    return findMinimum(data->A, data->N, data->n, data->evaluatedFunction, data->constaintFunction, &data->solutionFound, data->solution);
}

point_t *CalculationConcurrent_findMinimum(point_t **points, const int n, const int N, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction)
{
    pthread_t threads[NUM_THREADS];
    point_t *solution = NULL;
    solution = Point_Init(solution, n);

    concurrent_data_t data;
    data.A = points;
    data.N = N;
    data.n = n;
    data.evaluatedFunction = evaluatedFunction;
    data.constaintFunction = constaintFunction;
    data.solutionFound     = false;
    data.solution          = solution;

    for(int i = 0; i < NUM_THREADS; i++) {
        if(pthread_create(&threads[i], NULL, runCalculation, (void*)&data) > 0) {
            perror("Cannot create thread");
            return NULL;
        }
    }

    // wait until solution will be found
    while(!data.solutionFound);

    // if any thread find solution, join others
    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    solution = data.solution;
    return solution;
}
