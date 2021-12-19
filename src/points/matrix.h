#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

struct point_t;
typedef struct {
    int rows;
    int cols;
//    void **data;
    struct point_t **data;
} matrix_t;

matrix_t* Matrix_Init(const size_t rows, const size_t cols, const int argsCount);
void Matrix_Destroy(matrix_t *matrix);

void Matrix_PrintAll(const matrix_t *matrix);
struct point_t* Matrix_DataAt(const matrix_t *matrix, const int row, const int col);

#endif // MATRIX_H
