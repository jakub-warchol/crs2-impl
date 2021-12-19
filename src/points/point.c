#include "point.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

point_t *Point_Init(point_t *point, const int argsCount)
{
    if(point) {
        point->argCount = argsCount;
        point->args     = calloc(argsCount, sizeof(double));
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
        printf("Point(%p) is NULL!", (const void*) point);
    } else {
        printf("Point(count: %d, args:(", point->argCount);
        for(int i = 0; i < point->argCount - 1; i++) {
            printf("%f, ", point->args[i]);
        }

        // print last arg
        printf("%f))\n", point->args[point->argCount - 1]);
    }
}
