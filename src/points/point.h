#ifndef POINT_T_H
#define POINT_T_H

typedef struct point_t{
    int argCount;
    double *args;
} point_t;

point_t* Point_Init(point_t *point, const int argsCount);
void Point_Destroy(point_t *point);

void Point_Print(const point_t *point);

#endif // POINT_T_H
