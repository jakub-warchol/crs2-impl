#include "calculation_distributed.h"
#include "../points/point.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>
#include <unistd.h>

int nodesNumber = 0;

static bool continueCalculations(int thread){

    if (thread == nodesNumber){ // continue computing
        bool check = true;
        for(int i = 0; i<nodesNumber; i++) {
            MPI_Send(&check, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD); // send request to continue
        }
    }else{
        bool check;
        MPI_Recv(&check, 1, MPI_C_BOOL, nodesNumber, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // block on waiting for new data
        if(!check){
            return false;
        }
    }

    return true;
}

static void stopCalculations(int thread){
    if (thread == nodesNumber){ // finalize computing
        bool check = false;
        for(int i = 0; i<nodesNumber; i++) {
            MPI_Send(&check, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);
        }
    }
}

point_t *CalculationDistributed_findMinimum(point_t **points, const int n, const int N, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction)
{
    // set nodes count
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // set current node number
    int currentRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    nodesNumber = worldSize - 1;

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

        // find best and worst points
        M = A[0];
        L = A[0];

        if(!continueCalculations(currentRank)){
            stopCalculations(currentRank);
            return NULL;
        }

        if (currentRank == nodesNumber){ // the last node is synchronizer

            for(int i = 0; i<N; i++) {
                MPI_Send(&i, 1, MPI_INTEGER, i%nodesNumber, 0, MPI_COMM_WORLD); // send data to compute
            }

            double temp[2];
            for(int i = 0; i<N; i++) {
                MPI_Recv(&temp, 2, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive data

                point_t *it = A[(int)temp[0]];
                double value = temp[1];

                if (value < L->value){
                    L = it;
                }else if(value > M->value){
                    M = it;
                }
            }

        }else{ // other nodes - compute ranks
            for(int i = 0; i<N; i++) {
                if(i%nodesNumber == currentRank){ // check if current rank has the action to complete
                    int temp;
                    MPI_Recv(&temp, 1, MPI_DOUBLE, nodesNumber, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receinve data from main node

                    point_t *it = A[i];
                    double fValue[] = {(double)i,it->value}; // send idx nad value

                    MPI_Send(&fValue, 2, MPI_DOUBLE, nodesNumber, 0, MPI_COMM_WORLD); // send solution
                }
            }
        }

        if (currentRank == nodesNumber){
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

                // if f_p < f_m, swap points
                if(P->value < M->value) {
                    Point_Swap(P, M);
                    break;
                }
            }

            // create solution point
            solution = Point_Init(solution, n);
            memcpy(solution, L, sizeof(point_t));

            // check stop criterion
            if((L->value == 0. && M->value == 0.) || (M->value / L->value) < CLC_RESOLUTION) {
                goto cleanup;
                break; // congratulations, you've found the minimum!
            }else if (counter > CLC_MAX_ITERATIONS){
                printf("Max interations!\n");
                goto cleanup;
                break;
            }

        }

    }

cleanup:
    // remove G and P
    Point_Destroy(G);
    Point_Destroy(P);

    // free memory
    free(R);

    stopCalculations(currentRank);

    return solution;
}
