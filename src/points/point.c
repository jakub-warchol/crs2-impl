#include "point.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

point_t *Point_Init(point_t *point, const int argsCount)
{
    if(point == NULL) {
        point = malloc(sizeof(point_t));
    }

    if(point) {
        point->argCount = argsCount;
        point->args     = calloc(argsCount, sizeof(double));
        point->value    = 0.;
    }

    return point;
}

void Point_Destroy(point_t *point)
{
    if(point) {
        // destroy args
        free(point->args);

        // null pointer
        point = NULL;
    }
}

void Point_Print(const point_t *point)
{
    if(point == NULL) {
        printf("Point is NULL!\n");
    } else {
        printf("Point(args:(");
        for(int i = 0; i < point->argCount - 1; i++) {
            printf("%f, ", point->args[i]);
        }

        // print last arg
        printf("%f), value: %f)\n", point->args[point->argCount - 1], point->value);
    }

    fflush(stdout);
}

void Point_Swap(point_t *p1, point_t *p2)
{
    // create local temp copy of p1
    double value   = p1->value;
    int argsCount = p1->argCount;
    double *args = malloc(p1->argCount * sizeof(double));
    memcpy(args, p1->args, argsCount);

    // copy p2 to p1
    p1->argCount = p2->argCount;
    p1->value    = p2->value;
    memcpy(p1->args, p2->args, p1->argCount);

    // copy temp to p2
    p2->argCount = argsCount;
    p2->value    = value;
    memcpy(p2->args, args, argsCount);

    free(args);
}
