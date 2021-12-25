#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "src/crs2/calculation.h"
#include "src/points/point.h"

/// TODO: temporary macro
#define f(x1, x2) ((1 + ((x1) + (x2) + 1) * \
    ((x1) + (x2) + 1) * (19 - 14 * (x1) + 3 * (x1) * (x1) - 14 * (x2) + 6 * (x1) * (x2) + 3 * (x2) * (x2)) * \
    (19 - 14 * (x1) + 3 * (x1) * (x1) - 14 * (x2) + 6 * (x1) * (x2) + 3 * (x2) * (x2))) * \
    (30 + (2 * (x1) - 3 * (x2)) * (2 * (x1) - 3 * (x2)) * (18 - 32 * (x1) + 12 * (x1) * (x1) + \
    48 * (x2) - 36 * (x1) * (x2) + 27 * (x2) * (x2))))

double evaluatedFunction(const int n, double *args) {
    return f(args[0], args[1]);
}

int checkConstraints(int argNo, double value) {
    if(argNo == 0) {
        if(value >= -2) {
            return 0;
        } else {
            return -1;
        }
    } else if(argNo == 1) {
        if(value <= 2) {
            return 0;
        } else {
            return 1;
        }
    }

    return -1;
}

double generateValue(int argNo) {
    int sign = (rand() % 2 == 0) ? 1 : -1;
    double mul = 10;
    double value = sign * drand48() * mul;

    while(true) {
        int res = checkConstraints(argNo, value);
        if(res == 0) {
            break;
        } else if(res == 1) {
            mul /= 2;
        } else {
            mul *= 2;
        }

        sign  = (rand() % 2 == 0) ? 1 : -1;
        value = sign * drand48() * mul;
    }

    return value;
}

int main(int argc, char **argv)
{
    srand(time(NULL));

    const int n = 2; // no of variables
    const int N = 10 * (n + 1);

    point_t** points = calloc(N, sizeof(point_t*));
    for(int i = 0; i < N; i++) {
        point_t *point = NULL;
        point = Point_Init(point, n);

        for(int j = 0; j < n; j++) {
            double v = generateValue(j);
            point->args[j] = v;
        }

        points[i] = point;
        Point_Print(point);
    }

//    points[10]->args[0] = 0;
//    points[10]->args[1] = -1;

    printf("Found minimum: ");
    fflush(stdout);
    point_t *solution = Calculation_FindMinimum(points, n, evaluatedFunction, checkConstraints, Sequential);
    Point_Print(solution);

    for(int i = 0; i < N; i++) {
        Point_Destroy(points[i]);
    }
    free(points);

    return 0;
}
