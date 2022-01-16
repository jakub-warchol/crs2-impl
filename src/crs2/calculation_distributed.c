#include "calculation_distributed.h"
#include "../points/point.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>

typedef enum {
    SolutionFound = 100,
    MPointFound,
    CalculationAborted,
} openmpi_calc_tag_t;

static void sendPoint(point_t *point, int root) {
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // send new point
    for(int i = 0; i < worldSize; i++) {
        MPI_Send(point, sizeof (point_t), MPI_CHAR, i, MPointFound, MPI_COMM_WORLD);
    }

//    MPI_Bcast(point, sizeof (point_t), MPI_CHAR, root, MPI_COMM_WORLD);
}

static point_t* readPoint(int root) {
    point_t *data = NULL;
    int dataCount  = 0;
    int pointFound = 0;
    MPI_Status status;

    // check if point was sent
    MPI_Iprobe(MPI_ANY_SOURCE, MPointFound, MPI_COMM_WORLD, &pointFound, &status);
    if(pointFound) {
        MPI_Get_count(&status, MPI_CHAR, &dataCount);
        if(dataCount == sizeof (point_t) && status.MPI_SOURCE != root) { // if we read point and source is diffrent than current proc
            data = malloc(sizeof(point_t));
            MPI_Recv(data, sizeof (point_t), MPI_CHAR, MPI_ANY_SOURCE, MPointFound, MPI_COMM_WORLD, &status);
            return data;
        }
    }
    return NULL;
}

static void sendSolutionFound() {
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // send solution found
    int solutionFound = 1;
    for(int i = 0; i < worldSize; i++) {
        MPI_Send(&solutionFound, 1, MPI_INT, i, SolutionFound, MPI_COMM_WORLD);
    }
}

static bool checkSolutionFound() {
    int solutionFoundSent = 0;
    MPI_Status status;

    // check if point was sent
    MPI_Iprobe(MPI_ANY_SOURCE, MPointFound, MPI_COMM_WORLD, &solutionFoundSent, &status);
    if(solutionFoundSent) {
        int solutionFound = 0;
        MPI_Recv(&solutionFound, 1, MPI_INT, MPI_ANY_SOURCE, SolutionFound, MPI_COMM_WORLD, &status);
        return solutionFound;
    }

    return false;
}

point_t *CalculationDistributed_findMinimum(point_t **points, const int n, const int N, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction)
{
    // init MPI
    MPI_Init(NULL, NULL);

    // set nodes count
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // set current node number
    int currentRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    // do stuff
    // init some used below variables
    point_t* G = NULL;
    G = Point_Init(G, n);

    point_t* P = NULL;
    P = Point_Init(P, n);

    point_t *M  = NULL;
    point_t *L  = NULL;
    point_t *R  = calloc(n + 1, sizeof(point_t));
    point_t **A = points;
    point_t *solution = NULL;

    int counter = 0;
    while(true) {
        // if solution was already found, just break loop
        if(checkSolutionFound()) {
            goto cleanup;
        }

        // find best and worst points
        M = A[0];
        L = A[0];

        for(int i = 0; i < N; i++) {

            point_t *it = A[i];
            double fValue = it->value;

            if(fValue < L->value) {
                L = it;
            }else if(fValue > M->value) {
                M = it;
            }
        }

        while(true) {
            // create R's simplex
            R[0] = *L;

            for(int i = 1; i < n + 1;) {
                int idx = rand() % N;

                if(A[idx] != L) { // if chosen point is diffrent than the best
                    R[i] = *A[idx]; // add to simplex
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

            // if solution was already found, just break loop
            if(checkSolutionFound()) {
                goto cleanup;
            }

            // if f_p < f_m, swap points
            if(P->value < M->value) {
                Point_Swap(P, M);
                break;
            }
        }

        counter++;

        // check stop criterion
        if((L->value == 0. && M->value == 0.) || (M->value / L->value) < CLC_RESOLUTION) {
            sendSolutionFound();
            break; // congratulations, you've found the minimum!
        }else if (counter > CLC_MAX_ITERATIONS){
            printf("Max interations!\n");
            goto cleanup;
        }

    }

    // create solution point
    solution = Point_Init(solution, n);
    memcpy(solution, L, sizeof(point_t));

    cleanup:
    // remove G and P
    Point_Destroy(G);
    Point_Destroy(P);

    // free memory
    free(R);

    // clear MPI
    MPI_Finalize();

    return solution;
}
