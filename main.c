#include <stdio.h>

#include "src/points/matrix.h"
#include "src/points/point.h"

/// TODO: temporary macro
#define f(x1, x2) ((1 + ((x1) + (x2) + 1) * \
    ((x1) + (x2) + 1) * (19 - 14 * (x1) + 3 * (x1) * (x1) - 14 * (x2) + 6 * (x1) * (x2) + 3 * (x2) * (x2)) * \
    (19 - 14 * (x1) + 3 * (x1) * (x1) - 14 * (x2) + 6 * (x1) * (x2) + 3 * (x2) * (x2))) * \
    (30 + (2 * (x1) - 3 * (x2)) * (2 * (x1) - 3 * (x2)) * (18 - 32 * (x1) + 12 * (x1) * (x1) + \
    48 * (x2) - 36 * (x1) * (x2) + 27 * (x2) * (x2))))


int main(int argc, char **argv)
{
    matrix_t *m = Matrix_Init(10, 10, 2);
    Matrix_PrintAll(m);

    Matrix_Destroy(m);
    return 0;
}
