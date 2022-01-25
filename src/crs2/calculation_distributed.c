#include "calculation_distributed.h"
#include "../points/point.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>
#include <unistd.h>

typedef enum {
    SolutionFound = 100,
    MPointFound,
    CalculationAborted,
} openmpi_calc_tag_t;

int threadsNumber = 0;

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

static bool continueCalculations(int thread){

    if (thread == threadsNumber){ // kontynuacja obliczen!
        bool check = true;
        for(int i = 0; i<threadsNumber; i++) {
            MPI_Send(&check, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD); // wyslij continue
        }
    }else{
        bool check;
        MPI_Recv(&check, 1, MPI_C_BOOL, threadsNumber, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //odbierz continue!
        if(!check){
            return false;
        }
    }

    //MPI_Barrier(MPI_COMM_WORLD); //wyrownanie

    return true;

}

static void stopCalculations(int thread){

    //printf("end rank %d\n", thread);

    if (thread == threadsNumber){ // koniec obliczen!

        bool check = false;
        for(int i = 0; i<threadsNumber; i++) {
            MPI_Send(&check, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);
        }

    }

    //MPI_Finalize();

}

point_t *CalculationDistributed_findMinimum(point_t **points, const int n, const int N, evaluated_function_t evaluatedFunction, constraint_function_t constaintFunction)
{
    //printf("n: %d ",n);

    // init MPI
    //MPI_Init(NULL, NULL); // init w main :)

    // set nodes count
    int worldSize;
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // set current node number
    int currentRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    threadsNumber = worldSize - 1;

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

        if (currentRank == threadsNumber){ // watek ostatni synchronizuje dane

            for(int i = 0; i<N; i++) {
                MPI_Send(&i, 1, MPI_INTEGER, i%threadsNumber, 0, MPI_COMM_WORLD); // wyslij co maja liczyc watki
            }

            double temp[2];

            for(int i = 0; i<N; i++) {
                MPI_Recv(&temp, 2, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // odbierz gotowe wyniki

                //printf("Min value: %f ",L->value);
                //printf("Na: %d jest %f\n",(int)temp[0],temp[1]);

                point_t *it = A[(int)temp[0]];
                double value = temp[1];

                if (value < L->value){
                    L = it;
                }else if(value > M->value){
                    M = it;
                }
            }

        }else{ // wszystkie pozostale watki - watki liczace

            for(int i = 0; i<N; i++) {
                if(i%threadsNumber == currentRank){ // sprawdz czy dany watek ma wykonac dana operacje
                    int temp;
                    MPI_Recv(&temp, 1, MPI_DOUBLE, threadsNumber, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //odbierz dane od glownego watku

                    point_t *it = A[i];
                    double fValue[] = {(double)i,it->value}; // niestety nie mozna przeslac point_t, dlatego trzeba przesalc wartosc i oraz wyliczona funkcje :(

                    MPI_Send(&fValue, 2, MPI_DOUBLE, threadsNumber, 0, MPI_COMM_WORLD); // wyslij rozwiazane
                }
            }
        }

        if (currentRank == threadsNumber){
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

                //printf("args: %d",G->argCount);

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
