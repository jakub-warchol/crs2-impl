#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <stddef.h>
#include <mpi.h>

#include "src/crs2/calculation.h"
#include "src/points/point.h"
#include "src/tests/test_functions.h"

#define clrscr() printf("\e[1;1H\e[2J")

double generateValue(int argNo, constraint_function_t checkConstraints) {
    int sign = (rand() % 2 == 0) ? 1 : -1;
    double mul = 10;
    double value = sign * ((double)rand() / (double)RAND_MAX) * mul;

    while(true) {
        int res = checkConstraints(argNo, value);
        if(res == 0) {
            break;
        } else if(res == 1) {
            mul /= 2;
        } else {
            mul *= 2;
        }

        sign = (rand() % 2 == 0) ? 1 : -1;
        value = sign * ((double)rand() / (double)RAND_MAX) * mul;
    }

    return value;
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    const int n = 2; // no of variables
    const int N = 10 * (n + 1);

    // function which minimum is needed
    evaluated_function_t evaluatedFunction         = NULL;
    constraint_function_t checkConstraintsFunction = NULL;

    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int currentRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &currentRank);

    int option = 0;
    int threadsNumber = world_size - 1;

    if (currentRank == 0){
        if (argc < 3) {
            printf("Select number of function to be solved and confirm with enter: \n"
                   "1. Goldstein-Price \n"
                   "2. Levy \n"
                   "3. Eggholder \n"
                   "4. Drop Wave \n"
                   "5. McCormick \n");
            scanf("%d", &option);
        }else{
            option = strtol(argv[1], NULL, 10);
        }
        for(int i = 0; i<threadsNumber; i++) {
            MPI_Send(&option, 1, MPI_INTEGER, i+1, 0, MPI_COMM_WORLD); // wyslij continue
        }
    }else{
        MPI_Recv(&option, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //odbierz continue!
    }

    MPI_Barrier(MPI_COMM_WORLD); //wyrownanie

    switch (option){
        case 1:
            evaluatedFunction        = goldstein_price_function;
            checkConstraintsFunction = goldstein_price_constraint;
            break;
        case 2:
            evaluatedFunction        = levy_function;
            checkConstraintsFunction = levy_constraint;
            break;
        case 3:
            evaluatedFunction        = eggholder_function;
            checkConstraintsFunction = eggholder_constraint;
            break;
        case 4:
            evaluatedFunction        = drop_wave_function;
            checkConstraintsFunction = drop_wave_constraint;
            break;
        case 5:
            evaluatedFunction        = mccormick_function;
            checkConstraintsFunction = mccormick_constraint;
            break;
    }

    // prepare points array
    point_t** points = calloc(N, sizeof(point_t*));
    for(int i = 0; i < N; i++) {
        point_t *point = NULL;
        point = Point_Init(point, n);

        for(int j = 0; j < n; j++) {
            double v = generateValue(j, checkConstraintsFunction);
            point->args[j] = v;
        }

        points[i] = point;
    }

    if (argc < 3) {
        point_t *solutionSequential = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraintsFunction, Sequential);
        point_t *solutionParallel = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraintsFunction, Parallel);
        point_t *solutionDistribution;

        if(threadsNumber > 0){
            solutionDistribution = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraintsFunction, Distribution);
        }

        if (currentRank == threadsNumber) {
            clrscr();

            printf("Found minimum: \n");
            printf("Sequential: ");
            Point_Print(solutionSequential);

            printf("Parallel: ");
            Point_Print(solutionParallel);

            if(threadsNumber > 0){
                printf("Distribution: ");
                Point_Print(solutionDistribution);
            }

            fflush(stdout);
        }

    }else{
        point_t *solution;
        int solutionOption = strtol(argv[2], NULL, 10);
        switch(solutionOption){
            case 1:
                solution = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraintsFunction, Sequential);
                break;
            case 2:
                solution = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraintsFunction, Parallel);
                break;
            case 3:
                solution = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraintsFunction, Distribution);
                break;
            default:
                printf("Error");
                exit(EXIT_FAILURE);
        }

        if (currentRank == threadsNumber){
            printf("Found minimum: \n");
            Point_Print(solution);
        }

    }

    // clear
    for(int i = 0; i < N; i++) {
        Point_Destroy(points[i]);
    }
    free(points);

    MPI_Finalize();

    return 0;
}
