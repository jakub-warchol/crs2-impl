#include "matrix.h"
#include "point.h"

#include <malloc.h>
#include <stdio.h>


matrix_t *Matrix_Init(const size_t rows, const size_t cols, const int argsCount)
{
    matrix_t *self = NULL;

    if(rows > 0 && cols > 0) {
        self = malloc(sizeof(matrix_t));
        if(self) {
            // set constraints
            self->rows = (int)rows;
            self->cols = (int)cols;

            self->data = malloc(cols * sizeof(point_t*));
            if(self->data) {
                for(size_t i = 0; i < rows; i++) {
                    self->data[i] = malloc(cols * sizeof(point_t));

                    // init every cell
                    for(size_t j = 0; j < cols; j++) {
                        Point_Init(&self->data[i][j], argsCount);
                    }
                }
            }
        }
    }

    return self;
}

void Matrix_Destroy(matrix_t *matrix)
{
    if(matrix) {
        // destroy every row
        for(int i = 0; i < matrix->rows; i++) {

            // destroy every cell
            for(int j = 0; j < matrix->cols; j++) {
                Point_Destroy(&matrix->data[i][j]);
            }

            free(matrix->data[i]);
        }

        // destroy rows' array
        free(matrix->data);

        // destroy matrix
        free(matrix);

        // null destroyed pointer
        matrix = NULL;
    }
}

struct point_t *Matrix_DataAt(const matrix_t *matrix, const int row, const int col)
{
    if(matrix) {
        if(row > 0 && row < matrix->rows && col > 0 && col < matrix->cols) {
            return &matrix->data[row][col];
        }
    }

    return NULL;
}

void Matrix_PrintAll(const matrix_t *matrix)
{
    if(matrix == NULL) {
        printf("Matrix(%p) is NULL!\n", (const void*)matrix);
    } else {
        for(int i = 0; i < matrix->rows; i++) {
            for(int j = 0; j < matrix->cols; j++) {
                Point_Print(&matrix->data[i][j]);
            }
        }
    }
}
